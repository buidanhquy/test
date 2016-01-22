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

#define HASH_COUNT 10
#define MAX_RECORD 5

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


int find_idx_by_key(char key_arr[][255], char *key) {
    int i;
    for (i = 0; i < MAX_RECORD; i++) {
        if (0 == strcmp(key_arr[i], key)) {
            return i;
        }
    }
    return -1;
}

void load_database(opool_t *opool, hash_table_t *ht, char *table_name, db_t *database) {
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
    i = 0;
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
            ht_add_item(ht, (void *)database->key_arr[i], (void *)database->data_list[i]);
        }
        else {
            DL_APPEND(database->data_list[idx], temp);
        }
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
    sqlite3_stmt *stmt;
    int n;

    CALL_SQLITE (open (db_path, &db));

    n = sprintf(sql, "UPDATE %s SET passphrase = '%s' WHERE dev_id = '%s'", table_name, passphrase, id);
    sql[n] = '\0';

    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
    CALL_SQLITE_EXPECT (step (stmt), DONE);
}

//======================== update passphrase table ==================================//
void update_table(opool_t *opool, hash_table_t *ht, char *table_name, char *field_1, char *field_2) {
    ht_add_item(ht, (void *)field_1, (void *)field_2);
    update_database(opool, ht, table_name, field_1, field_2); 
    //load_database(opool, ht, "table_name", field_1, field_2);
    //load_database(opool, ht, "passphrase", char key_arr[][255], entry_t *data_list[MAX_RECORD]) {
}

void show_database(hash_table_t *ht, char *key);

int main() {
    hash_table_t ht_permission;
    hash_table_t ht_login;
    hash_table_t ht_passphrase;

    //char p_key[MAX_RECORD][255], login_key[MAX_RECORD][255], pp_key[MAX_RECORD][255];
    //entry_t *p_data[MAX_RECORD], *login_data[MAX_RECORD], *pp_data[MAX_RECORD];
    db_t permission_db;

    permission_db.data_list = malloc(MAX_RECORD*sizeof(entry_t *));

    pj_status_t status;
    pj_caching_pool cp;
    pj_pool_t *pool;
    opool_t opool;

    CHECK(__FILE__, pj_init());
    SET_LOG_LEVEL(4);
    pj_log_set_level(3);    

    pj_caching_pool_init(&cp, NULL, 1024);

    pool = pj_pool_create(&cp.factory, "pool", 1024, 1024, NULL);

    opool_init(&opool, 40, sizeof(entry_t), pool);

    //INIT HASH TABLE
    ht_init(&ht_permission, pool);
    //ht_init(&ht_login, pool);
    //ht_init(&ht_passphrase, pool);

    //CREATE HASH TABLE
    ht_create(&ht_permission, HASH_COUNT);
    //ht_create(&ht_login, HASH_COUNT);
    //ht_create(&ht_passphrase, HASH_COUNT);

    //=========== LOAD DATA FROM DB THEN INSERT INTO HASH TABLE ==========//
    load_database(&opool, &ht_permission, "permission", &permission_db);
    //load_database(&opool, &ht_login, "login", login_key, login_data);
    //load_database(&opool, &ht_passphrase, "passphrase", pp_key, pp_data);

    //======================= PROCESS DATA FROM HASH TABLE================//
    //HT_permission sample:
    // Acc_1 | RIUC11, RIUC12
    // RIUC11 | RIUC11, Acc_1
    //entry_t *temp2, *entry;
    
    printf("=============== Permission =============\n");
    show_database(&ht_permission, "Acc_1");
    show_database(&ht_permission, "Acc_2");
#if 0
    printf("=============== Login =============\n");
    //show_database(&ht_login, "Acc_1");
    show_database(&ht_login, "Acc_2");

    printf("=============== Passphrase =============\n");
    show_database(&ht_passphrase, "OIUC1");
    show_database(&ht_passphrase, "OIUC2");
    show_database(&ht_passphrase, "RIUC11");
    show_database(&ht_passphrase, "RIUC12");

    update_table(&opool, &ht_passphrase, "passphrase", "OIUC2", "qwerty");
    //show_database(&ht_passphrase, "OIUC2");
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
    

        
    //==================== check_permission_table ========================//
    //Already has sender_id (user_id in permission_table)
    strcpy(role_data, (char *)ht_get_item(&ht_permission, "Acc_1"));
    printf("User_id: %s - Role_data: %s\n", "Acc_1", role_data);

    role_id = strtok(role_data, ", ");

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
    return 0;
}

void show_database(hash_table_t *ht, char *key) {
    entry_t *data_list;
    entry_t *temp, *entry;

    data_list = (entry_t *)ht_get_item(ht, (void *)key);

    printf("Key: %s - Value:", key);
    DL_FOREACH_SAFE(data_list, temp, entry) {
        printf(" %s |", temp->value);
    }
    printf("\n");
}
