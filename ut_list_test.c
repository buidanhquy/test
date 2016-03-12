#include "ansi-utils.h"
#include "utlist.h"

typedef struct entry_s entry_t;

struct entry_s {
    entry_t *next;
    entry_t *prev;
};

typedef struct db_s db_t;

struct db_s {
    entry_t *data_list[10];
};

int main() {    
    int i;
    entry_t *temp, *entry;
    entry_t *temp2, *entry2;
    
    db_t db;

#if 0
    for (i = 0; i < 10; i++) {
        db.data_list[i] = malloc(sizeof(entry_t));
    }
#endif

    for (i = 0; i < 10; i++) {
        DL_FOREACH_SAFE(db.data_list[i], temp, entry) {
            DL_DELETE(db.data_list[i], temp);
        }
    }

    for (i = 0; i < 10; i++) {
        //DL_FOREACH_SAFE(data_list2, temp2, entry2) {}
    }

    return 0;
}

