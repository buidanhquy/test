#include <stdio.h>
#include <stdlib.h>
#include <pjlib.h>
#include "hash-table.h"
#include "my-pjlib-utils.h"
#include "ansi-utils.h"
#include "mysqlite.h"
#include <sqlite3.h>
#include "object-pool.h"
#include "utlist.h"
#include "proto-constants.h"

#define HASH_COUNT 10
#define MAX_RECORD 50

char db_path[] = "./databases/permission.db";

typedef struct entry_s entry_t;

struct entry_s {
    char value[100];

    entry_t *next;
    entry_t *prev;
};

typedef struct db_s db_t;

struct db_s {
    char key_arr[MAX_RECORD][255];
    entry_t *data_list[MAX_RECORD];
};

db_t permission_db;
db_t login_db;
db_t passphrase_db;

int find_idx_by_key(char key_arr[][255], char *key) {
    int i;
    for (i = 0; i < MAX_RECORD; i++) {
        if (0 == strcmp(key_arr[i], key)) {
            return i;
        }
    }
    return -1;
}

int load_database(opool_t *opool, hash_table_t *ht, char *table_name, db_t *database) {
    sqlite3 *db;
    char sql[50], key[10], key_temp[10];
    sqlite3_stmt *stmt;
    int i, idx, n;

    opool_item_t *item = NULL;

    entry_t *temp, *entry;
 
    //==================== Open database ================================//   
    CALL_SQLITE (open (db_path, &db));

    n = sprintf(sql ,"SELECT *FROM %s", table_name);

    //==================== Delele previous data ========================//   
    for (i = 0; i < MAX_RECORD; i++) {
        DL_FOREACH_SAFE(database->data_list[i], temp, entry) {
            DL_DELETE(database->data_list[i], temp);
        }
    }

    i = 0;
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (entry_t *)item->data;

        ansi_copy_str(key_temp, (char *)sqlite3_column_text(stmt, 0));       
        ansi_copy_str(temp->value, (char *)sqlite3_column_text(stmt, 1));
        idx = find_idx_by_key(database->key_arr, key_temp);
        if (idx < 0) {
            ansi_copy_str(database->key_arr[i], key_temp);
            DL_APPEND(database->data_list[i], temp);
        }
        else {
            DL_APPEND(database->data_list[idx], temp);
        }
        ht_add_item(ht, (void *)database->key_arr[i], (void *)database->data_list[i]);
        i++;
    }
    return i;
}

void load_database_reverse(opool_t *opool, hash_table_t *ht, char *table_name, db_t *database, int key_arr_size) {
    sqlite3 *db;
    char sql[50], key[10], key_temp[10];
    sqlite3_stmt *stmt;
    int i, idx, n;

    opool_item_t *item = NULL;

    entry_t *temp;
 
    //====================================================//   
    CALL_SQLITE (open (db_path, &db));
    n = sprintf(sql ,"SELECT *FROM %s", table_name);
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
    //====================================================//   
    i = key_arr_size;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (entry_t *)item->data;

        ansi_copy_str(key_temp, (char *)sqlite3_column_text(stmt, 1));       
        ansi_copy_str(temp->value, (char *)sqlite3_column_text(stmt, 0));
        idx = find_idx_by_key(database->key_arr, key_temp);
        if (idx < 0) {
            ansi_copy_str(database->key_arr[i], key_temp);
            DL_APPEND(database->data_list[i], temp);
        }
        else {
            DL_APPEND(database->data_list[idx], temp);
        }
        ht_add_item(ht, (void *)database->key_arr[i], (void *)database->data_list[i]);
        i++;
    }
}
#if 0
        key | value
        for i = 0; i < MAX_RECORD; i++
            ht_add_item(key[i], data_list[i]);
        i = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            temp = sqlite3_column_text(stmt, 0));
            value = sqlite3_colum_text(stmt, 1));
            check temp in key[MAX_RECORD] => return idx
            if idx < 0
                key[i] = temp;
                appen valua into data_list[i];
            else
                append value into data_list[idx];
            i++;
        }
#endif       

//============================ update_database =======================================//
void update_database(opool_t *opool, hash_table_t *ht, char *table_name, char *id, char *passphrase) {
    sqlite3 *db;
    char sql[255], key[10], key_temp[10];
    char column1[10], column2[10];
    sqlite3_stmt *stmt;
    int n;

    CALL_SQLITE (open (db_path, &db));

    n = sprintf(sql, "SELECT * FROM %s ", table_name);
    sql[n] = '\0';
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));

    ansi_copy_str(column1, (char *)sqlite3_column_name(stmt, 0));
    ansi_copy_str(column2, (char *)sqlite3_column_name(stmt, 1));

    n = sprintf(sql, "SELECT * FROM %s WHERE %s = '%s' ", table_name, column1, id);
    sql[n] = '\0';
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));

    n = sprintf(sql, "UPDATE %s SET %s = '%s' WHERE %s = '%s'", table_name, column2, passphrase, column1, id);
    sql[n] = '\0';
    SHOW_LOG(3, "%s\n", sql);

    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
    CALL_SQLITE_EXPECT (step (stmt), DONE);
}

//======================== update passphrase table ==================================//
void update_table(opool_t *opool, hash_table_t *ht, char *table_name, db_t *db, char *field_1, char *field_2) {
    update_database(opool, ht, table_name, field_1, field_2); 
    load_database(opool, ht, table_name, &passphrase_db);
}

void show_record(hash_table_t *ht, char *key) {
    entry_t *data_list;
    entry_t *temp, *entry;

    data_list = (entry_t *)ht_get_item(ht, (void *)key);

    printf("Key: %s - Value:", key);

    DL_FOREACH_SAFE(data_list, temp, entry) {
        printf(" %s |", temp->value);
    }
    printf("\n");
}

entry_t *get_data_list(hash_table_t *ht, char *key) {
    return ht_get_item(ht, (void *)key);
}

int main() {
    int key_arr_size;

    hash_table_t ht_permission;
    hash_table_t ht_login;
    hash_table_t ht_passphrase;

    pj_status_t status;
    pj_caching_pool cp;
    pj_pool_t *pool;
    opool_t opool;

    //INIT POOL
    CHECK(__FILE__, pj_init());
    SET_LOG_LEVEL(4);
    pj_log_set_level(3);    

    pj_caching_pool_init(&cp, NULL, 1024);
    pool = pj_pool_create(&cp.factory, "pool", 1024, 1024, NULL);
    opool_init(&opool, 40, sizeof(entry_t), pool);

    //INIT HASH TABLE
    ht_init(&ht_permission, pool);
    ht_init(&ht_login, pool);
    ht_init(&ht_passphrase, pool);

    //CREATE HASH TABLE
    ht_create(&ht_permission, HASH_COUNT);
    ht_create(&ht_login, HASH_COUNT);
    ht_create(&ht_passphrase, HASH_COUNT);

    //=========== LOAD DATA FROM DB THEN INSERT INTO HASH TABLE ==========//
    key_arr_size = load_database(&opool, &ht_permission, "permission", &permission_db);
    load_database_reverse(&opool, &ht_permission, "permission", &permission_db, key_arr_size);
    load_database(&opool, &ht_login, "login", &login_db);
    load_database(&opool, &ht_passphrase, "passphrase", &passphrase_db);

    //======================= PROCESS DATA FROM HASH TABLE================//
    //HT_permission sample:
    // Acc_1 | RIUC11, RIUC12
    // RIUC11 | RIUC11, Acc_1
    //entry_t *temp2, *entry;
    
    printf("=============== Permission =============\n");
    show_record(&ht_permission, "Acc_1");
    show_record(&ht_permission, "Acc_2");
    show_record(&ht_permission, "RIUC11");
    show_record(&ht_permission, "RIUC12");
    printf("=============== Login =============\n");
    show_record(&ht_login, "Acc_1");
    show_record(&ht_login, "Acc_2");

    printf("=============== Passphrase =============\n");
    show_record(&ht_passphrase, "OIUC1");
    show_record(&ht_passphrase, "OIUC2");
    show_record(&ht_passphrase, "RIUC11");
    show_record(&ht_passphrase, "RIUC12");
    
    update_table(&opool, &ht_passphrase, "passphrase", &passphrase_db, "OIUC2", "123512312");
    //update_table(&opool, &ht_passphrase, "passphrase", &passphrase_db, "COORD", COORD_PP);
    show_record(&ht_passphrase, "OIUC2");
    //show_record(&ht_passphrase, "COORD");

    //==================== check_permission_table && send passphrase ========================//
    //Already has sender_id (user_id in permission_table)
    char user_id[] = "Acc_1";

    entry_t *temp, *temp2, *entry, *entry2;
    entry_t *passphrase;
    entry_t *data_list = get_data_list(&ht_permission, user_id);

    printf("Received GM_REG with user_id: %s\n", user_id);
    DL_FOREACH_SAFE(data_list, temp, entry) {
        passphrase = get_data_list(&ht_passphrase, temp->value);
        DL_FOREACH_SAFE(passphrase, temp2, entry2) {
            printf("Send passphrase: %s of %s to %s\n", temp2->value, temp->value, user_id );
        }
    }
    //=========================================================================================//

#if 0
    printf("User_id: %s - Role_data: %s\n", "Acc_1", role_data);

    while (role_id != NULL) {
        printf("ID %d: %s\n", i, role_id);
        //if role_id is account (eg. "Acc_1")
            //get dev_id from ht_login by role_id
            //then get passphrase from ht_passphrase by dev_id
        //if role_id is device
            //get passphrase from ht_passphrase by role_id

        //Got passphrase!
        //Send passphrase
        role_id = strtok(NULL, ", ");
        i++;
    }
#endif
#if 0
    GM_REG request comming
        if node is new
            update_passphrase_table
            check_permission_table
            send_passphrase

         if node is already in table
            check_passphrase_table
                if passphrase change
                    update_passphrase_table
                    check_permission_table
                    send_passphrase
                else
                    do nothing
#endif

#if 0
#endif
    return 0;
}
