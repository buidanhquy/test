#include "utlist.h"
#include "object-pool.h"
#include <pjlib.h>
#include "ansi-utils.h"
#include "my-pjlib-utils.h"

typedef struct entry_s entry_t;

struct entry_s {
    char value[100];

    entry_t *next;
    entry_t *prev;
};

void test(opool_t *opool, entry_t **entry) {
    opool_item_t *item = NULL;
    entry_t *temp, *entry2;

    item = opool_get(opool);
    EXIT_IF_TRUE(item == NULL, "Cannot get from object pool\n");
    temp = (entry_t *)item->data;

    strcpy(temp->value, "1234");
    DL_APPEND(*entry, temp);
    DL_FOREACH_SAFE(*entry, temp, entry2) {
        printf("temp: %s\n", temp->value);
    }
}

int main() {

    pj_caching_pool cp;
    pj_pool_t *pool;
    opool_t opool;
    
    entry_t *list, *entry, *temp;

    //INIT POOL
    CHECK(__FILE__, pj_init());
    SET_LOG_LEVEL(4);
    pj_log_set_level(3);    

    pj_caching_pool_init(&cp, NULL, 1024);
    pool = pj_pool_create(&cp.factory, "pool", 1024, 1024, NULL);
    opool_init(&opool, 100, sizeof(entry_t), pool);

    test(&opool, &list);
    
    printf("==========\n");

    DL_FOREACH_SAFE(list, temp, entry) {
        printf("temp: %s\n", temp->value);
    }

    return 0;
}
