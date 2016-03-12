#include "permission.h"
#include "json.h"

void init_passphrase_db(passphrase_db_t *db, pj_pool_t *pool, char *db_path) {
    if (db_path != NULL) {
        ansi_copy_str(db->db_path, db_path);
    }
    opool_init(&db->opool, 100, sizeof(perm_node_t), pool);
}
void init_login_db(login_db_t *db, pj_pool_t *pool, char *db_path) {
    if (db_path != NULL) {
        ansi_copy_str(db->db_path, db_path);
    }
    opool_init(&db->opool, 100, sizeof(perm_node_t), pool);
}

void init_permission_db(permission_db_t *db, pj_pool_t *pool, char *db_path) {
    if (db_path != NULL) {
        ansi_copy_str(db->db_path, db_path);
    }
    opool_init(&db->opool, 100, sizeof(perm_node_t), pool);
}
void init_asterisk_db(asterisk_acc_db_t *db, pj_pool_t *pool, char *db_path, char*sip_conf_path) {
    if (db_path != NULL && sip_conf_path != NULL) {
        ansi_copy_str(db->db_path, db_path);
        ansi_copy_str(db->sip_conf_path, sip_conf_path);
    }
    opool_init(&db->opool, 100, sizeof(perm_node_t), pool);
}

void load_login_db(login_db_t *login_db) {
    sqlite3 *db;
    char sql[50], key_temp[10];
    sqlite3_stmt *stmt;
    int n;

    opool_item_t *item, *item2;

    perm_node_t *temp, *data_list;
    perm_node_t *temp2;

    data_list = NULL;
    item = NULL;
    item2 = NULL;

    //==================== Open database ================================//   
    CALL_SQLITE (open (login_db->db_path, &db));

    n = sprintf(sql ,"SELECT * FROM login");
    sql[n] = '\0';
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
    //===================== LOAD DATABASE (LEFT TO RIGHT) =====================================//
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(&login_db->opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (perm_node_t *)item->data;

        ansi_copy_str(key_temp, (char *)sqlite3_column_text(stmt, 0)); 
        ansi_copy_str(temp->value, (char *)sqlite3_column_text(stmt, 1));
        data_list = (perm_node_t *)ht_get_item(&login_db->login_table, (void *)key_temp);
        if (data_list == NULL) {
            item2 = opool_get(&login_db->opool);
            EXIT_IF_TRUE(item2 == NULL, "Cannot get from object pool\n");
            temp2 = (perm_node_t *)item2->data;

            ansi_copy_str(temp2->value, (char *)sqlite3_column_text(stmt, 0));

            DL_APPEND(login_db->key_list, temp2);

            DL_APPEND(data_list, temp);
            ht_add_item(&login_db->login_table, (void *)sqlite3_column_text(stmt, 0), data_list);
            opool_free(&login_db->opool, item2);
        }
        else {
            DL_APPEND(data_list, temp);
        }

        opool_free(&login_db->opool, item);
    }
}

void load_passphrase_db(passphrase_db_t *passphrase_db) {
    sqlite3 *db;
    char sql[50], key_temp[10];
    sqlite3_stmt *stmt;
    int n;

    opool_item_t *item, *item2;

    perm_node_t *temp, *data_list;
    perm_node_t *temp2;

    data_list = NULL;
    item = NULL;
    item2 = NULL;

    //==================== Open database ================================//   
    CALL_SQLITE (open (passphrase_db->db_path, &db));

    n = sprintf(sql ,"SELECT * FROM passphrase");
    sql[n] = '\0';
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
    //===================== LOAD DATABASE (LEFT TO RIGHT) =====================================//
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(&passphrase_db->opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (perm_node_t *)item->data;

        ansi_copy_str(key_temp, (char *)sqlite3_column_text(stmt, 0)); 
        ansi_copy_str(temp->value, (char *)sqlite3_column_text(stmt, 1));
        data_list = (perm_node_t *)ht_get_item(&passphrase_db->passphrase_table, (void *)key_temp);
        if (data_list == NULL) {
            item2 = opool_get(&passphrase_db->opool);
            EXIT_IF_TRUE(item2 == NULL, "Cannot get from object pool\n");
            temp2 = (perm_node_t *)item2->data;

            ansi_copy_str(temp2->value, (char *)sqlite3_column_text(stmt, 0));

            DL_APPEND(passphrase_db->key_list, temp2);

            DL_APPEND(data_list, temp);
            ht_add_item(&passphrase_db->passphrase_table, (void *)sqlite3_column_text(stmt, 0), data_list);
            opool_free(&passphrase_db->opool, item2);
        }
        else {
            DL_APPEND(data_list, temp);
        }
        opool_free(&passphrase_db->opool, item);
    }
}

void load_asterisk_acc_db(asterisk_acc_db_t *asterisk_db) {
    sqlite3 *db;
    char sql[50], key_temp[10];
    sqlite3_stmt *stmt;
    int n;

    opool_item_t *item, *item2;

    perm_node_t *temp, *data_list;
    perm_node_t *temp2;

    data_list = NULL;
    item = NULL;
    item2 = NULL;

    //==================== Open database ================================//   
    CALL_SQLITE (open (asterisk_db->db_path, &db));

    n = sprintf(sql ,"SELECT * FROM ast_sipfriends");
    sql[n] = '\0';
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
    //===================== LOAD DATABASE (LEFT TO RIGHT) =====================================//
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(&asterisk_db->opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (perm_node_t *)item->data;

        ansi_copy_str(key_temp, (char *)sqlite3_column_text(stmt, 1)); 
        ansi_copy_str(temp->value, (char *)sqlite3_column_text(stmt, 2));
        data_list = (perm_node_t *)ht_get_item(&asterisk_db->asterisk_acc_table, (void *)key_temp);
        if (data_list == NULL) {
            item2 = opool_get(&asterisk_db->opool);
            EXIT_IF_TRUE(item2 == NULL, "Cannot get from object pool\n");
            temp2 = (perm_node_t *)item2->data;

            ansi_copy_str(temp2->value, (char *)sqlite3_column_text(stmt, 1));

            DL_APPEND(asterisk_db->key_list, temp2);

            DL_APPEND(data_list, temp);
            ht_add_item(&asterisk_db->asterisk_acc_table, (void *)key_temp, data_list);
            opool_free(&asterisk_db->opool, item2);
        }
        else {
            DL_APPEND(data_list, temp);
        }
        opool_free(&asterisk_db->opool, item);
    }
}

void load_permission_db(permission_db_t *permission_db) {
    sqlite3 *db;
    char sql[50], key_temp[10];
    sqlite3_stmt *stmt;
    int n;

    opool_item_t *item, *item2;

    perm_node_t *temp, *data_list;
    perm_node_t *temp2;

    data_list = NULL;
    item = NULL;
    item2 = NULL;

    //==================== Open database ================================//   
    CALL_SQLITE (open (permission_db->db_path, &db));

    n = sprintf(sql ,"SELECT * FROM permission");
    sql[n] = '\0';
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));

    //===================== LOAD DATABASE (LEFT TO RIGHT) =====================================//
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(&permission_db->opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (perm_node_t *)item->data;

        ansi_copy_str(key_temp, (char *)sqlite3_column_text(stmt, 0)); 
        ansi_copy_str(temp->value, (char *)sqlite3_column_text(stmt, 1));
        data_list = (perm_node_t *)ht_get_item(&permission_db->permission_table, (void *)key_temp);
        if (data_list == NULL) {
            item2 = opool_get(&permission_db->opool);
            EXIT_IF_TRUE(item2 == NULL, "Cannot get from object pool\n");
            temp2 = (perm_node_t *)item2->data;

            ansi_copy_str(temp2->value, (char *)sqlite3_column_text(stmt, 0));

            DL_APPEND(permission_db->key_list, temp2);

            DL_APPEND(data_list, temp);
            ht_add_item(&permission_db->permission_table, (void *)sqlite3_column_text(stmt, 0), data_list);
            opool_free(&permission_db->opool, item2);
        }
        else {
            DL_APPEND(data_list, temp);
        }
        opool_free(&permission_db->opool, item);
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(&permission_db->opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (perm_node_t *)item->data;

        ansi_copy_str(key_temp, (char *)sqlite3_column_text(stmt, 1)); 
        ansi_copy_str(temp->value, (char *)sqlite3_column_text(stmt, 0));
        data_list = (perm_node_t *)ht_get_item(&permission_db->permission_table, (void *)key_temp);
        if (data_list == NULL) {
            item2 = opool_get(&permission_db->opool);
            EXIT_IF_TRUE(item2 == NULL, "Cannot get from object pool\n");
            temp2 = (perm_node_t *)item2->data;

            ansi_copy_str(temp2->value, (char *)sqlite3_column_text(stmt, 1));

            DL_APPEND(permission_db->key_list, temp2);

            DL_APPEND(data_list, temp);
            ht_add_item(&permission_db->permission_table, (void *)sqlite3_column_text(stmt, 1), data_list);
            opool_free(&permission_db->opool, item2);
        }
        else {
            DL_APPEND(data_list, temp);
        }
        opool_free(&permission_db->opool, item);
    }
}

static int value_cmp(perm_node_t *n1, perm_node_t *n2) {
    return strncmp(n1->value, n2->value, sizeof(n2->value));
}
//======================== update table ==================================//
int add_passphrase(passphrase_db_t *db, char *node_id, char *passphrase) {
    opool_item_t *item = NULL, *item2 = NULL;

    perm_node_t *temp, *temp2, *to_return;

    perm_node_t *data_list = NULL;

    item = opool_get(&db->opool);
    EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
    temp = (perm_node_t *)item->data;

    printf("UPDATE TABLE passphrase VALUES('%s', '%s')\n", node_id, passphrase);

    ansi_copy_str(temp->value, passphrase);

    data_list = (perm_node_t *)ht_get_item(&db->passphrase_table, (void *)node_id);

    if (data_list == NULL) {
        item2 = opool_get(&db->opool);
        EXIT_IF_TRUE(item2 == NULL, "Cannot get from object pool\n");
        temp2 = (perm_node_t *)item2->data;

        ansi_copy_str(temp2->value, node_id);

        DL_APPEND(db->key_list, temp2);

        DL_APPEND(data_list, temp);
        ht_add_item(&db->passphrase_table, (void *)node_id, (void *)data_list);    
        opool_free(&db->opool, item2);
    }
    else {
        DL_SEARCH(data_list, to_return, temp, value_cmp);
        if (to_return != NULL) {
            SHOW_LOG(3, "Record: %s - %s has already in the table passphrase!\n", node_id, passphrase);
            return -1;
        }
        else {
                DL_REPLACE_ELEM(data_list, data_list, temp);
                ht_add_item(&db->passphrase_table, (void *)node_id, (void *)data_list);    
            }
    }
    opool_free(&db->opool, item);
    return 0;
}

int add_active_acc(login_db_t *db, char *user_id, char *node_id) {
    opool_item_t *item = NULL, *item2 = NULL;

    perm_node_t *temp, *temp2, *to_return;

    perm_node_t *data_list = NULL;

    item = opool_get(&db->opool);
    EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
    temp = (perm_node_t *)item->data;

    printf("UPDATE TABLE login VALUES('%s', '%s')\n", user_id, node_id);

    ansi_copy_str(temp->value, node_id);

    data_list = (perm_node_t *)ht_get_item(&db->login_table, (void *)user_id);

    if (data_list == NULL) {
        item2 = opool_get(&db->opool);
        EXIT_IF_TRUE(item2 == NULL, "Cannot get from object pool\n");
        temp2 = (perm_node_t *)item2->data;

        ansi_copy_str(temp2->value, user_id);

        DL_APPEND(db->key_list, temp2);

        DL_APPEND(data_list, temp);
        ht_add_item(&db->login_table, (void *)user_id, (void *)data_list);    
        opool_free(&db->opool, item2);
    }
    else {
        DL_SEARCH(data_list, to_return, temp, value_cmp);
        if (to_return != NULL) {
            SHOW_LOG(3, "Record: %s - %s has already in the table login!\n", user_id, node_id);
            return -1;
        }
        else {
                DL_REPLACE_ELEM(data_list, data_list, temp);
                ht_add_item(&db->login_table, (void *)user_id, (void *)data_list);    
            }
    }   
    opool_free(&db->opool, item);
    return 0;
}

int add_asterisk_acc(asterisk_acc_db_t *asterisk_db, char *account, char *password) {
    opool_item_t *item = NULL, *item2 = NULL;

    perm_node_t *temp, *temp2, *to_return;

    perm_node_t *data_list = NULL;

    item = opool_get(&asterisk_db->opool);
    EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
    temp = (perm_node_t *)item->data;

    //printf("UPDATE TABLE asterisk_acount VALUES('%s', '%s')\n", account, password);

    ansi_copy_str(temp->value, password);

    data_list = (perm_node_t *)ht_get_item(&asterisk_db->asterisk_acc_table, (void *)account);

    if (data_list == NULL) {
        item2 = opool_get(&asterisk_db->opool);
        EXIT_IF_TRUE(item2 == NULL, "Cannot get from object pool\n");
        temp2 = (perm_node_t *)item2->data;

        ansi_copy_str(temp2->value, account);

        DL_APPEND(asterisk_db->key_list, temp2);

        DL_APPEND(data_list, temp);
        ht_add_item(&asterisk_db->asterisk_acc_table, (void *)account, (void *)data_list);    
        opool_free(&asterisk_db->opool, item2);
    }
    else {
        DL_SEARCH(data_list, to_return, temp, value_cmp);
        if (to_return != NULL) {
            SHOW_LOG(3, "Record: %s - %s has already in the table!\n", account, password);
            return -1;
        }
        else {
                DL_REPLACE_ELEM(data_list, data_list, temp);
                ht_add_item(&asterisk_db->asterisk_acc_table, (void *)account, (void *)data_list);    
            }
    }
    opool_free(&asterisk_db->opool, item);
    return 0;
}

static int _grant_access(permission_db_t *db, char *user_id, char *node_id) {
    opool_item_t *item = NULL, *item2 = NULL;

    perm_node_t *temp, *temp2, *to_return;

    perm_node_t *data_list = NULL;

    item = opool_get(&db->opool);
    EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
    temp = (perm_node_t *)item->data;

    printf("UPDATE TABLE permission VALUES('%s', '%s')\n", user_id, node_id);

    ansi_copy_str(temp->value, node_id);

    data_list = (perm_node_t *)ht_get_item(&db->permission_table, (void *)user_id);

    if (data_list == NULL) {
        item2 = opool_get(&db->opool);
        EXIT_IF_TRUE(item2 == NULL, "Cannot get from object pool\n");
        temp2 = (perm_node_t *)item2->data;

        ansi_copy_str(temp2->value, user_id);

        DL_APPEND(db->key_list, temp2);

        DL_APPEND(data_list, temp);
        ht_add_item(&db->permission_table, (void *)user_id, (void *)data_list);    
        opool_free(&db->opool, item2);
    }
    else {
        DL_SEARCH(data_list, to_return, temp, value_cmp);
        if (to_return != NULL) {
            SHOW_LOG(3, "Record: %s - %s has already in the table permission!\n", user_id, node_id);
            return -1;
        }
        else {
                DL_APPEND(data_list, temp);
                ht_add_item(&db->permission_table, (void *)user_id, (void *)data_list);    
            }
    }
    opool_free(&db->opool, item);
    return 0;
}
int grant_access(permission_db_t *db, char *user_id, char *node_id) {
    int ret1, ret2;
    ret1 = _grant_access(db, user_id, node_id);
    ret2 = _grant_access(db, node_id, user_id);

    return (ret1 + ret2);
}

static int is_change_login(login_db_t *this_db, char *acc, char*node) {
    sqlite3 *db;
    char sql[50];
    char acc_tmp[20], node_tmp[20];
    sqlite3_stmt *stmt;
    int n, is_exist = 2;

    opool_item_t *item, *item2;

    perm_node_t *temp, *data_list;
    perm_node_t *temp2;

    data_list = NULL;
    item = NULL;
    item2 = NULL;

    //==================== Open database ================================//   
    CALL_SQLITE (open (this_db->db_path, &db));

    n = sprintf(sql ,"SELECT * FROM login");
    sql[n] = '\0';
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
#if 1
    //===================== LOAD DATABASE (LEFT TO RIGHT) =====================================//
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(&this_db->opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (perm_node_t *)item->data;

        ansi_copy_str(acc_tmp, (char *)sqlite3_column_text(stmt, 0)); 
        ansi_copy_str(node_tmp, (char *)sqlite3_column_text(stmt, 1));

        if (0 == strcmp(acc_tmp, acc)) {
            is_exist = 1;
        }

        if (0 == strcmp(acc_tmp, acc) && 0 == strcmp(node_tmp, node)) {
            is_exist = 0;
        }
        opool_free(&this_db->opool, item);
    }
#endif
    return is_exist;
}

static int is_change_permission(permission_db_t *this_db, char *acc, char*node) {
    sqlite3 *db;
    char sql[50];
    char acc_tmp[20], node_tmp[20];
    sqlite3_stmt *stmt;
    int n, is_exist = 2;

    opool_item_t *item, *item2;

    perm_node_t *temp, *data_list;
    perm_node_t *temp2;

    data_list = NULL;
    item = NULL;
    item2 = NULL;

    //==================== Open database ================================//   
    CALL_SQLITE (open (this_db->db_path, &db));

    n = sprintf(sql ,"SELECT * FROM permission");
    sql[n] = '\0';
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
#if 1
    //===================== LOAD DATABASE (LEFT TO RIGHT) =====================================//
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(&this_db->opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (perm_node_t *)item->data;

        ansi_copy_str(acc_tmp, (char *)sqlite3_column_text(stmt, 0)); 
        ansi_copy_str(node_tmp, (char *)sqlite3_column_text(stmt, 1));

        if (0 == strcmp(acc_tmp, acc) && 0 == strcmp(node_tmp, node)) {
            is_exist = 0;
        }
        opool_free(&this_db->opool, item);
    }
#endif
    return is_exist;
}

static int is_change_passphrase(passphrase_db_t *this_db, char *node, char*pph) {
    sqlite3 *db;
    char sql[50];
    char node_tmp[20], pph_tmp[20];
    sqlite3_stmt *stmt;
    int n, is_exist = 2;

    opool_item_t *item, *item2;

    perm_node_t *temp, *data_list;
    perm_node_t *temp2;

    data_list = NULL;
    item = NULL;
    item2 = NULL;

    //==================== Open database ================================//   
    CALL_SQLITE (open (this_db->db_path, &db));

    n = sprintf(sql ,"SELECT * FROM passphrase");
    sql[n] = '\0';
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
#if 1
    //===================== LOAD DATABASE (LEFT TO RIGHT) =====================================//
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(&this_db->opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (perm_node_t *)item->data;

        ansi_copy_str(node_tmp, (char *)sqlite3_column_text(stmt, 0)); 
        ansi_copy_str(pph_tmp, (char *)sqlite3_column_text(stmt, 1));

        if (0 == strcmp(node_tmp, node)) {
            is_exist = 1;
        }

        if (0 == strcmp(node_tmp, node) && 0 == strcmp(pph_tmp, pph)) {
            is_exist = 0;
        }
        opool_free(&this_db->opool, item);
    }
#endif
    return is_exist;
}

static int is_change_asterisk_acc(asterisk_acc_db_t *asterisk_db, char *acc, char*pwd) {
    sqlite3 *db;
    char sql[50];
    char acc_tmp[20], pwd_tmp[20];
    sqlite3_stmt *stmt;
    int n, is_exist = 2;

    opool_item_t *item, *item2;

    perm_node_t *temp, *data_list;
    perm_node_t *temp2;

    data_list = NULL;
    item = NULL;
    item2 = NULL;

    printf("acc: %s - pwd: %s\n", acc, pwd);

    //==================== Open database ================================//   
    CALL_SQLITE (open (asterisk_db->db_path, &db));

    n = sprintf(sql ,"SELECT * FROM ast_sipfriends");
    sql[n] = '\0';
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
#if 1
    //===================== LOAD DATABASE (LEFT TO RIGHT) =====================================//
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(&asterisk_db->opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (perm_node_t *)item->data;

        ansi_copy_str(acc_tmp, (char *)sqlite3_column_text(stmt, 1)); 
        ansi_copy_str(pwd_tmp, (char *)sqlite3_column_text(stmt, 2));

        if (0 == strcmp(acc_tmp, acc)) {
            is_exist = 1;
        }

        if (0 == strcmp(acc_tmp, acc) && 0 == strcmp(pwd_tmp, pwd)) {
            is_exist = 0;
        }
        opool_free(&asterisk_db->opool, item);
    }
#endif
    return is_exist;
}

void save_login_db(login_db_t *login_db) {
    sqlite3 *db;
    char sql[255];
    sqlite3_stmt *stmt;
    int n;
    perm_node_t *temp, *entry;
    perm_node_t *to_return;
    perm_node_t *temp2, *entry2;
    perm_node_t *value_list;

    char acc_tmp[20], node_tmp[20];
    opool_item_t *item;

    int is_change = 0;

    DL_FOREACH_SAFE(login_db->key_list, temp, entry) {
        value_list = (perm_node_t *)ht_get_item(&login_db->login_table, (void *)temp->value);

        if (value_list != NULL) {
            DL_FOREACH_SAFE(value_list, temp2, entry2) {
                is_change = is_change_login(login_db, temp->value, temp2->value);
                CALL_SQLITE (open (login_db->db_path, &db));
                switch (is_change) {
                    case 0:
                        SHOW_LOG(1, "No Change: %s - %s\n", temp->value, temp2->value);
                        break;
                    case 1:
                        SHOW_LOG(1, "Update: %s - %s\n", temp->value, temp2->value);
                        CALL_SQLITE (open (login_db->db_path, &db));
                        n = sprintf(sql, "UPDATE login SET dev_id = '%s' WHERE acc_id = '%s'", temp2->value, temp->value);              
                        //printf("%s\n", sql);
                        CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
                        CALL_SQLITE_EXPECT (step (stmt), DONE);
                        break;
                    case 2:
                        SHOW_LOG(1, "Insert: %s - %s\n", temp->value, temp2->value);
                        CALL_SQLITE (open (login_db->db_path, &db));
                        n = sprintf(sql, "INSERT INTO login VALUES ('%s', '%s')", temp->value, temp2->value);
                        //printf("%s\n", sql);
                        CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
                        CALL_SQLITE_EXPECT (step (stmt), DONE);
                        break;
                    default:
                        SHOW_LOG(1, "Unknown result: %d\n", is_change);
                        break;   
                }
            }
        }
        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }
    }
    //FOR REMOVE RECORD PURPOSE
    CALL_SQLITE (open (login_db->db_path, &db));

    n = sprintf(sql ,"SELECT * FROM login");
    sql[n] = '\0';
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
#if 1
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(&login_db->opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (perm_node_t *)item->data;

        ansi_copy_str(acc_tmp, (char *)sqlite3_column_text(stmt, 0));
        ansi_copy_str(node_tmp, (char *)sqlite3_column_text(stmt, 1));

        ansi_copy_str(temp->value, acc_tmp);
        DL_SEARCH(login_db->key_list, to_return, temp, value_cmp);   
        if (to_return == NULL) {
            SHOW_LOG(1, "Remove: %s - %s\n", acc_tmp, node_tmp);

            //=========== DELETE RECORD OF TABLE ===================//
            n = sprintf(sql, "DELETE FROM login WHERE acc_id = '%s'", acc_tmp);
            sql[n] = '\0';
            //printf("%s\n", sql);
            CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
            CALL_SQLITE_EXPECT (step (stmt), DONE);        
        }
        opool_free(&login_db->opool, item);
    }
#endif
}

void save_passphrase_db(passphrase_db_t *passphrase_db) {
    sqlite3 *db;
    char sql[255];
    sqlite3_stmt *stmt;
    int n;
    perm_node_t *temp, *entry;
    perm_node_t *to_return;
    perm_node_t *temp2, *entry2;
    perm_node_t *value_list;

    char node_tmp[20], pph_tmp[20];
    opool_item_t *item;

    int is_change = 0;

    DL_FOREACH_SAFE(passphrase_db->key_list, temp, entry) {
        value_list = (perm_node_t *)ht_get_item(&passphrase_db->passphrase_table, (void *)temp->value);

        if (value_list != NULL) {
            DL_FOREACH_SAFE(value_list, temp2, entry2) {
                is_change = is_change_passphrase(passphrase_db, temp->value, temp2->value);
                CALL_SQLITE (open (passphrase_db->db_path, &db));
                switch (is_change) {
                    case 0:
                        SHOW_LOG(1, "No Change: %s - %s\n", temp->value, temp2->value);
                        break;
                    case 1:
                        SHOW_LOG(1, "Update: %s - %s\n", temp->value, temp2->value);
                        CALL_SQLITE (open (passphrase_db->db_path, &db));
                        n = sprintf(sql, "UPDATE passphrase SET passphrase = '%s' WHERE dev_id = '%s'", temp2->value, temp->value);              
                        //printf("%s\n", sql);
                        CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
                        CALL_SQLITE_EXPECT (step (stmt), DONE);
                        break;
                    case 2:
                         SHOW_LOG(1, "Insert: %s - %s\n", temp->value, temp2->value);
                        CALL_SQLITE (open (passphrase_db->db_path, &db));
                        n = sprintf(sql, "INSERT INTO passphrase VALUES ('%s', '%s')", temp->value, temp2->value);
                        //printf("%s\n", sql);
                        CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
                        CALL_SQLITE_EXPECT (step (stmt), DONE);
                        break;
                    default:
                        SHOW_LOG(1, "Unknown result: %d\n", is_change);
                        break;   
                }
            }
        }
        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }

    }
    //FOR REMOVE RECORD PURPOSE
    CALL_SQLITE (open (passphrase_db->db_path, &db));

    n = sprintf(sql ,"SELECT * FROM passphrase");
    sql[n] = '\0';
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
#if 1
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(&passphrase_db->opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (perm_node_t *)item->data;

        ansi_copy_str(node_tmp, (char *)sqlite3_column_text(stmt, 0));
        ansi_copy_str(pph_tmp, (char *)sqlite3_column_text(stmt, 1));

        ansi_copy_str(temp->value, node_tmp);
        DL_SEARCH(passphrase_db->key_list, to_return, temp, value_cmp);   
        if (to_return == NULL) {
            SHOW_LOG(1, "Remove: %s - %s\n", node_tmp, pph_tmp);

            //=========== DELETE RECORD OF TABLE ===================//
            n = sprintf(sql, "DELETE FROM passphrase WHERE dev_id = '%s'", node_tmp);
            sql[n] = '\0';
            //printf("%s\n", sql);
            CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
            CALL_SQLITE_EXPECT (step (stmt), DONE);        
        }

        opool_free(&passphrase_db->opool, item);
    }
#endif
}
void save_permission_db(permission_db_t *permission_db) {
    sqlite3 *db;
    char sql[255];
    sqlite3_stmt *stmt;
    int n;
    perm_node_t *temp, *entry;
    perm_node_t *to_return;
    perm_node_t *temp2, *entry2;
    perm_node_t *value_list;

    char user_tmp[20], node_tmp[20];
    opool_item_t *item;

    int is_change = 0;

    DL_FOREACH_SAFE(permission_db->key_list, temp, entry) {
        value_list = (perm_node_t *)ht_get_item(&permission_db->permission_table, (void *)temp->value);

        if (value_list != NULL) {
            DL_FOREACH_SAFE(value_list, temp2, entry2) {
                is_change = is_change_permission(permission_db, temp->value, temp2->value);
                CALL_SQLITE (open (permission_db->db_path, &db));
                switch (is_change) {
                    case 0:
                        SHOW_LOG(1, "No Change: %s - %s\n", temp->value, temp2->value);
                        break;
                    case 2:
                        SHOW_LOG(1, "Insert: %s - %s\n", temp->value, temp2->value);
                        CALL_SQLITE (open (permission_db->db_path, &db));
                        n = sprintf(sql, "INSERT INTO permission VALUES ('%s', '%s')", temp->value, temp2->value);
                        //printf("%s\n", sql);
                        CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
                        CALL_SQLITE_EXPECT (step (stmt), DONE);
                        break;
                    default:
                        SHOW_LOG(1, "Unknown result: %d\n", is_change);
                        break;   
                }
            }
        }
        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }

    }
    //FOR REMOVE RECORD PURPOSE
    CALL_SQLITE (open (permission_db->db_path, &db));

    n = sprintf(sql ,"SELECT * FROM permission");
    sql[n] = '\0';
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
#if 0
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(&permission_db->opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (perm_node_t *)item->data;

        ansi_copy_str(user_tmp, (char *)sqlite3_column_text(stmt, 0));
        ansi_copy_str(node_tmp, (char *)sqlite3_column_text(stmt, 1));

        ansi_copy_str(temp->value, user_tmp);
        DL_SEARCH(permission_db->key_list, to_return, temp, value_cmp);   
        if (to_return == NULL) {
            SHOW_LOG(1, "Remove: %s - %s\n", user_tmp, node_tmp);

            //=========== DELETE RECORD OF TABLE ===================//
            n = sprintf(sql, "DELETE FROM permission WHERE user_id = '%s'", user_tmp);
            sql[n] = '\0';
            //printf("%s\n", sql);
            CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
            CALL_SQLITE_EXPECT (step (stmt), DONE);        
        }
        opool_free(&permission_db->opool, item);
    }
#endif
}

void save_asterisk_acc_db(asterisk_acc_db_t *acc_db) {
    sqlite3 *db;
    char sql[255];
    char acc_tmp[20], pwd_tmp[20];
    sqlite3_stmt *stmt;
    int n;
    perm_node_t *temp, *entry;
    perm_node_t *to_return = NULL;
    opool_item_t *item;
    perm_node_t *temp2, *entry2;
    perm_node_t *value_list;

    int is_change = 0;

    CALL_SQLITE (open (acc_db->db_path, &db));

    DL_FOREACH_SAFE(acc_db->key_list, temp, entry) {
        value_list = (perm_node_t *)ht_get_item(&acc_db->asterisk_acc_table, (void *)temp->value);

        if (value_list != NULL) {
            //DL_FOREACH_SAFE(value_list, temp2, entry2) {
                printf("===Acc: %s - pass: %s\n", temp->value, value_list->value);
                is_change = is_change_asterisk_acc(acc_db, temp->value, value_list->value);
                switch (is_change) {
                    case 0:
                        SHOW_LOG(1, "No Change: %s - %s\n", temp->value, value_list->value);
                        break;
#if 1
                    case 1:
                        SHOW_LOG(1, "Update: %s - %s\n", temp->value, temp2->value);
                        n = sprintf(sql, "UPDATE ast_sipfriends SET secret = '%s' WHERE name = '%s'", value_list->value, temp->value);              
                        printf("%s\n", sql);
                        CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
                        CALL_SQLITE_EXPECT (step (stmt), DONE);
                        break;
                    case 2:
                         SHOW_LOG(1, "Insert: %s - %s\n", temp->value, value_list->value);
                        n = sprintf(sql, "INSERT INTO ast_sipfriends (type, name, defaultuser, host, secret, context) values ('friend', '%s', '%s', 'dynamic', '%s', 'internal')", temp->value, temp->value, value_list->value);              
                        //printf("%s\n", sql);
                        CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
                        CALL_SQLITE_EXPECT (step (stmt), DONE);
                        break;
                    default:
                        SHOW_LOG(1, "Unknown result: %d\n", is_change);
                        break;   
#endif
                }
            //}
        }
        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }
    }

    //FOR REMOVE RECORD PURPOSE

#if 0
    n = sprintf(sql ,"SELECT * FROM ast_sipfriends");
    sql[n] = '\0';
    CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        item = opool_get(&acc_db->opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        temp = (perm_node_t *)item->data;

        ansi_copy_str(acc_tmp, (char *)sqlite3_column_text(stmt, 1));
        ansi_copy_str(pwd_tmp, (char *)sqlite3_column_text(stmt, 2));

        ansi_copy_str(temp->value, acc_tmp);
        DL_SEARCH(acc_db->key_list, to_return, temp, value_cmp);   
        if (to_return == NULL) {
            SHOW_LOG(1, "Remove: %s - %s\n", acc_tmp, pwd_tmp);

            //=========== DELETE RECORD OF TABLE ===================//
            n = sprintf(sql, "DELETE FROM ast_sipfriends WHERE name = '%s'", acc_tmp);
            sql[n] = '\0';
            printf("%s\n", sql);
            CALL_SQLITE (prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL));
            CALL_SQLITE_EXPECT (step (stmt), DONE);        
        }
        opool_free(&acc_db->opool, item);
    }
#endif
}

int remove_passphrase(passphrase_db_t *pph_db, char *node_id) {
    sqlite3 *db;
    char sql[255];
    sqlite3_stmt *stmt;
    int n;

    perm_node_t *data_list;
    perm_node_t *temp;
    perm_node_t *to_return;
    opool_item_t *item;

    SHOW_LOG(3, "REMOVE key %s of table passphrase\n", node_id);

    item = opool_get(&pph_db->opool);
    EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
    temp = (perm_node_t *)item->data;

#if 1
    ansi_copy_str(temp->value, node_id);
    DL_SEARCH(pph_db->key_list, to_return, temp, value_cmp);   
    if (to_return != NULL) {
        DL_DELETE(pph_db->key_list, to_return); 
    }
    else {
        SHOW_LOG(3, "No record for key: %s\n", node_id);
        return -1;
    }
#endif
    data_list = (perm_node_t *)ht_get_item(&pph_db->passphrase_table, (void *)node_id);   

    if (data_list != NULL) {
        DL_DELETE(data_list, data_list);

        ht_add_item(&pph_db->passphrase_table, (void *)node_id, NULL);
    }
    else {
        SHOW_LOG(3, "ht_get_item return null\n");
        return -1;
    }
    opool_free(&pph_db->opool, item);
    return 0;
}

int remove_active_acc(login_db_t *login_db, char *user_id) {
    sqlite3 *db;
    char sql[255];
    sqlite3_stmt *stmt;
    int n;

    perm_node_t *data_list;
    perm_node_t *temp;
    perm_node_t *to_return;
    opool_item_t *item;

    SHOW_LOG(3, "REMOVE key %s of table login\n", user_id);

    item = opool_get(&login_db->opool);
    EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
    temp = (perm_node_t *)item->data;

#if 1
    ansi_copy_str(temp->value, user_id);
    DL_SEARCH(login_db->key_list, to_return, temp, value_cmp);   
    if (to_return != NULL) {
        DL_DELETE(login_db->key_list, to_return); 
    }
    else {
        SHOW_LOG(3, "No record for key: %s\n", user_id);
        return -1;
    }
#endif
    data_list = (perm_node_t *)ht_get_item(&login_db->login_table, (void *)user_id);   

    if (data_list != NULL) {
        DL_DELETE(data_list, data_list);

        ht_add_item(&login_db->login_table, (void *)user_id, NULL);
    }
    else {
        SHOW_LOG(3, "ht_get_item return null\n");
        return -1;
    }
    opool_free(&login_db->opool, item);
    return 0;
}

int remove_asterisk_acc(asterisk_acc_db_t *ast_db, char *account) {
    sqlite3 *db;
    char sql[255];
    sqlite3_stmt *stmt;
    int n;

    perm_node_t *data_list;
    perm_node_t *temp;
    perm_node_t *to_return;
    opool_item_t *item;

    SHOW_LOG(3, "REMOVE key %s of table asterisk_account\n", account);

    item = opool_get(&ast_db->opool);
    EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
    temp = (perm_node_t *)item->data;

#if 1
    ansi_copy_str(temp->value, account);
    DL_SEARCH(ast_db->key_list, to_return, temp, value_cmp);   
    if (to_return != NULL) {
        DL_DELETE(ast_db->key_list, to_return); 
    }
    else {
        SHOW_LOG(3, "No record for key: %s\n", account);    
        return -1;
    }
#endif
    data_list = (perm_node_t *)ht_get_item(&ast_db->asterisk_acc_table, (void *)account);   

    if (data_list != NULL) {
        DL_DELETE(data_list, data_list);

        ht_add_item(&ast_db->asterisk_acc_table, (void *)account, NULL);
    }
    else {
        SHOW_LOG(3, "ht_get_item return null\n");
        return -1;
    }
    opool_free(&ast_db->opool, item);
    return 0;
}
static int _revoke_access(permission_db_t *per_db, char *key, char *value) {
    sqlite3 *db;
    char sql[255];
    sqlite3_stmt *stmt;
    int n;

    perm_node_t *data_list;
    perm_node_t *temp;
    perm_node_t *to_return;
    opool_item_t *item;

    SHOW_LOG(3, "REMOVE ACCESS: %s -> %s\n", key, value);

    item = opool_get(&per_db->opool);
    EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
    temp = (perm_node_t *)item->data;

    ansi_copy_str(temp->value, value);
    data_list = (perm_node_t *)ht_get_item(&per_db->permission_table, (void *)key);   

    if (data_list != NULL) {
        DL_SEARCH(data_list, to_return, temp, value_cmp);   
        if (to_return != NULL) {
            DL_DELETE(data_list, to_return); 
        }
        else {
            SHOW_LOG(3, "No record for node id: %s\n", value);
            return -1;
        }

        ht_add_item(&per_db->permission_table, (void *)key, data_list);  
    }
    else {
        SHOW_LOG(3, "ht_get_item return null\n");
        return -1;
    }
    
#if 1
    //CHECK REMAINING RECORD BELONG TO KEY
    ansi_copy_str(temp->value, key);
    data_list = (perm_node_t *)ht_get_item(&per_db->permission_table, (void *)key);   

    DL_SEARCH(per_db->key_list, to_return, temp, value_cmp);   
    if (to_return != NULL) {
        if (data_list == NULL) {
            DL_DELETE(per_db->key_list, to_return); 
        }
    }
    else {
        SHOW_LOG(3, "No record for key: %s\n", key);
        return -1;
    }
#endif
    opool_free(&per_db->opool, item);
    return 0;
}

int revoke_access(permission_db_t *db, char *user_id, char *node_id) {
    int ret1, ret2;
    ret1 = _revoke_access(db, user_id, node_id);
    ret2 = _revoke_access(db, node_id, user_id);

    return (ret1 + ret2);
}

void show_passphrase_table(passphrase_db_t *db) {
    perm_node_t *temp, *entry;
    perm_node_t *temp2, *entry2;
    perm_node_t *data_list;

    DL_FOREACH_SAFE(db->key_list, temp, entry) {
        data_list = (perm_node_t *)ht_get_item(&db->passphrase_table, (void *)temp->value);
        if (data_list != NULL) {
            printf("Key: %s :", temp->value);
            DL_FOREACH_SAFE(data_list, temp2, entry2) {
                printf(" %s |", temp2->value);
            }
            printf("\n");
        }
        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }
    }

}
void show_login_table(login_db_t *db) {
    perm_node_t *temp, *entry;
    perm_node_t *temp2, *entry2;
    perm_node_t *data_list;

    DL_FOREACH_SAFE(db->key_list, temp, entry) {
        data_list = (perm_node_t *)ht_get_item(&db->login_table, (void *)temp->value);
        if (data_list != NULL) {
            printf("Key: %s :", temp->value);
            DL_FOREACH_SAFE(data_list, temp2, entry2) {
                printf(" %s |", temp2->value);
            }
            printf("\n");
        }
        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }
    }

}

void show_permission_table(permission_db_t *db) {
    perm_node_t *temp, *entry;
    perm_node_t *temp2, *entry2;
    perm_node_t *data_list;

    DL_FOREACH_SAFE(db->key_list, temp, entry) {
        data_list = (perm_node_t *)ht_get_item(&db->permission_table, (void *)temp->value);
        if (data_list != NULL) {
            printf("Key: %s :", temp->value);
            DL_FOREACH_SAFE(data_list, temp2, entry2) {
                printf(" %s |", temp2->value);
            }
            printf("\n");
        }
        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }
    }

}

void show_asterisk_acc_table(asterisk_acc_db_t *db) {
    perm_node_t *temp, *entry;
    perm_node_t *temp2, *entry2;
    perm_node_t *data_list;

    DL_FOREACH_SAFE(db->key_list, temp, entry) {
        data_list = (perm_node_t *)ht_get_item(&db->asterisk_acc_table, (void *)temp->value);
        if (data_list != NULL) {
            printf("%s | ", temp->value);
            DL_FOREACH_SAFE(data_list, temp2, entry2) {
                printf("%s", temp2->value);
            }
            printf("\n");
        }
        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }
    }

}

void show_permission_table_json(permission_db_t *db) {
    perm_node_t *temp, *entry;
    perm_node_t *temp2, *entry2;
    perm_node_t *data_list;

    json_object *jobj;
    char buffer[512];

    DL_FOREACH_SAFE(db->key_list, temp, entry) {
        data_list = (perm_node_t *)ht_get_item(&db->permission_table, (void *)temp->value);
        if (data_list != NULL) {
            //printf("Key: %s :", temp->value);
            DL_FOREACH_SAFE(data_list, temp2, entry2) {

                jobj = json_object_new_object();

                json_object_object_add(jobj, "acc_id", json_object_new_string(temp->value));
                json_object_object_add(jobj, "node_id", json_object_new_string(temp2->value));

                strncpy(buffer, json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PLAIN), sizeof(buffer));

                printf("%s", buffer);
    
                json_object_put(jobj);

                //printf(" %s |", temp2->value);
                printf("\n");
            }
        }
        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }
    }

}

void show_login_table_json(login_db_t *db) {
    perm_node_t *temp, *entry;
    perm_node_t *temp2, *entry2;
    perm_node_t *data_list;

    json_object *jobj;
    char buffer[512];

    DL_FOREACH_SAFE(db->key_list, temp, entry) {
        data_list = (perm_node_t *)ht_get_item(&db->login_table, (void *)temp->value);
        if (data_list != NULL) {
            //printf("Key: %s :", temp->value);
            DL_FOREACH_SAFE(data_list, temp2, entry2) {

                jobj = json_object_new_object();

                json_object_object_add(jobj, "acc_id", json_object_new_string(temp->value));
                json_object_object_add(jobj, "node_id", json_object_new_string(temp2->value));

                strncpy(buffer, json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PLAIN), sizeof(buffer));

                printf("%s", buffer);
    
                json_object_put(jobj);

                //printf(" %s |", temp2->value);
                printf("\n");
            }
        }   
        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }
    }

}

void show_asterisk_acc_table_json(asterisk_acc_db_t *db) {
    perm_node_t *temp, *entry;
    perm_node_t *temp2, *entry2;
    perm_node_t *data_list;

    json_object *jobj;
    char buffer[512];

    DL_FOREACH_SAFE(db->key_list, temp, entry) {
        data_list = (perm_node_t *)ht_get_item(&db->asterisk_acc_table, (void *)temp->value);
        if (data_list != NULL) {
            //printf("Key: %s :", temp->value);
            DL_FOREACH_SAFE(data_list, temp2, entry2) {

                jobj = json_object_new_object();

                json_object_object_add(jobj, "acc_id", json_object_new_string(temp->value));
                json_object_object_add(jobj, "node_id", json_object_new_string(temp2->value));

                strncpy(buffer, json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PLAIN), sizeof(buffer));

                printf("%s", buffer);
    
                json_object_put(jobj);

                //printf(" %s |", temp2->value);
                printf("\n");
            }
        }        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }
    }

}

void show_passphrase_table_json(passphrase_db_t *db) {
    perm_node_t *temp, *entry;
    perm_node_t *temp2, *entry2;
    perm_node_t *data_list;

    json_object *jobj;
    char buffer[512];

    DL_FOREACH_SAFE(db->key_list, temp, entry) {
        data_list = (perm_node_t *)ht_get_item(&db->passphrase_table, (void *)temp->value);
        if (data_list != NULL) {
            //printf("Key: %s :", temp->value);
            DL_FOREACH_SAFE(data_list, temp2, entry2) {

                jobj = json_object_new_object();

                json_object_object_add(jobj, "node_id", json_object_new_string(temp->value));
                json_object_object_add(jobj, "pph", json_object_new_string(temp2->value));

                strncpy(buffer, json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PLAIN), sizeof(buffer));

                printf("%s", buffer);
    
                json_object_put(jobj);

                //printf(" %s |", temp2->value);
                printf("\n");
            }
        }
        else {
            SHOW_LOG(3, "ht_get_item return null: key = %s\n", temp->value);
        }
    }

}

char *get_passphrase(passphrase_db_t *passphrase_db, char *node_id) {
    perm_node_t *temp, *entry;
    perm_node_t *data_list;

    data_list = (perm_node_t *)ht_get_item(&passphrase_db->passphrase_table, (void *)node_id);

    if (data_list != NULL) {
        return data_list->value;
    }
    else
        return NULL;
}

int get_passphrase_table_size(passphrase_db_t *db) {
    int size = 0;
    perm_node_t *temp, *entry;

    DL_FOREACH_SAFE(db->key_list, temp, entry) {
        size++;
    }   
    SHOW_LOG(5,"passphrase table size: %d\n", size);
    return size;
}
