#include "pjlib.h"
#include <pjsua-lib/pjsua.h>

pj_status_t status;
pj_caching_pool cp;
pj_pool_t *pool;

static FILE *log_file;

static void app_log_writer(int level, const char *buffer, int len)
{
    if (level <= 3) {
        pj_log_write(level, buffer, len);

        if (log_file) {
            fwrite(buffer, len, 1, log_file);
            fflush(log_file);
        }
    }
}

static void app_logging_init(void) {
    log_file = fopen("output", "wr");
    pj_log_set_log_func(&app_log_writer);
}

static void app_logging_shutdown(void) {
    if (log_file) {
        pj_file_close(log_file);
        log_file = NULL;
    }
}

int main() {
    
    status = pj_init();
    pj_caching_pool_init(&cp, NULL, 1024);
    pool = pj_pool_create(&cp.factory, "pool", 6400, 6400, NULL);

    app_logging_init();
    PJ_LOG(3, (__FILE__, "zzzzzz %d", 5));
    PJ_LOG(4, (__FILE__, "mmmmmm"));

    app_logging_shutdown();   

    return 0;
}
