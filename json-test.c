#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mysqlite.h"
#include <sqlite3.h>
#include "object-pool.h"
#include "utlist.h"
#include "proto-constants.h"
#include "permission.h"

permission_db_t permission_db;
permission_db_t permission_db_tmp;

void pack(char *buff, perm_node_t *key_list, permission_table_t permission_table) {
    perm_node_t *key, *entry;
    perm_node_t *value, *entry2;
    perm_node_t *data_list;

    int i;

    json_object *my_array;
	my_array = json_object_new_array();

    DL_FOREACH_SAFE(key_list, key, entry) {
        data_list = (perm_node_t *)ht_get_item(&permission_table, (void *)key->value);
        if (data_list != NULL) {
            DL_FOREACH_SAFE(data_list, value, entry2) {
                json_object *obj;

                obj = json_object_new_object();
                json_object_object_add(obj, "user_id", json_object_new_string(key->value));
                json_object_object_add(obj, "node_id", json_object_new_string(value->value));

                json_object_array_add(my_array, obj);
            }
        }
        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }
    }

#if 0
	printf("my_array 1 =\n");
	for(i=0; i < json_object_array_length(my_array); i++)
	{
		json_object *obj = json_object_array_get_idx(my_array, i);
		printf("\t[%d]=%s\n", i, json_object_to_json_string(obj));
	}

	printf("my_array.to_string()=%s\n", json_object_to_json_string(my_array));
#endif
    strcpy(buff, json_object_to_json_string(my_array));
}

void unpack(char *buff, perm_node_t **key_list, permission_table_t *permission_table, opool_t *opool) {
    int i;
    json_object *new_obj, *jvalue;
    opool_item_t *item, *item2;
    perm_node_t *key_element, *value_element, *data_list;

    perm_node_t *temp, *entry;
    perm_node_t *temp2, *entry2;
    perm_node_t *data_list2;

    char id[10], pwd[10];

	new_obj = json_tokener_parse(buff);
	//printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));

    for(i = 0; i < json_object_array_length(new_obj); i++)
	{ 
        item = opool_get(opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        key_element = (perm_node_t *)item->data;

        item2 = opool_get(opool);
        EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
        value_element = (perm_node_t *)item2->data;

        json_object *obj = json_object_array_get_idx(new_obj, i);

        json_object_object_get_ex(obj, "user_id", &jvalue);
        strcpy(id, json_object_get_string(jvalue));

        json_object_object_get_ex(obj, "node_id", &jvalue);
        strcpy(pwd, json_object_get_string(jvalue));

        ansi_copy_str(key_element->value, id);
        ansi_copy_str(value_element->value, pwd);

        data_list = (perm_node_t *)ht_get_item(permission_table, (void *)id);
        if (data_list == NULL) {
            DL_APPEND(*key_list, key_element);
            DL_APPEND(data_list, value_element);
            ht_add_item(permission_table, id, data_list);
        }
        else {
            DL_APPEND(data_list, value_element);
        }
    }
}

int main() {
	json_object *my_string, *my_int, *my_object, *my_array, *obj1, *obj2, *obj3;
    int i;
    char buff[512];

    pj_caching_pool cp;
    pj_pool_t *pool;

    char db_path[] = "../permission/databases/permission.db";

    pj_init();
    SET_LOG_LEVEL(4);
    pj_log_set_level(3);    
    pj_caching_pool_init(&cp, 0, 1024);
    pool = pj_pool_create(&cp.factory, "pool", 1024, 1024, NULL);

    init_permission_db(&permission_db, pool, db_path);
    init_permission_db(&permission_db_tmp, pool, db_path);

    //INIT HASH TABLE
    ht_init(&permission_db.permission_table, pool);
    ht_init(&permission_db_tmp.permission_table, pool);

    //CREATE HASH TABLE
    ht_create(&permission_db.permission_table, HASH_COUNT);
    ht_create(&permission_db_tmp.permission_table, HASH_COUNT);

    load_permission_db(&permission_db);

    show_permission_table(&permission_db);

    pack(buff, permission_db.key_list, permission_db.permission_table);

    printf("=============== DONE PACK =============\n");

    unpack(buff, &permission_db_tmp.key_list, &permission_db_tmp.permission_table, &permission_db_tmp.opool);

    perm_node_t *temp, *entry;
    perm_node_t *temp2, *entry2;
    perm_node_t *data_list;

    DL_FOREACH_SAFE(permission_db_tmp.key_list, temp, entry) {
        data_list = (perm_node_t *)ht_get_item(&permission_db_tmp.permission_table, (void *)temp->value);
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

    return 0;
}
