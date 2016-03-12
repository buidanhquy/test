#ifndef __RESPONSE_CLIENT_H__
#define __RESPONSE_CLIENT_H__
#include <stdarg.h>
#include <pjlib.h>
#include "lvcode.h"
#include "response.h"

#include "mysqlite.h"
#include <sqlite3.h>
#include "object-pool.h"
#include "utlist.h"
#include "proto-constants.h"
#include "permission.h"

typedef struct response_client_s response_client_t;

struct response_client_s {
	pj_sock_t fd;
	char connect_str[30];
    void *connect_data;
    void (*on_response_f)(response_client_t *uclient, response_request_t *req);
    char *(*id_f)();
    char *(*passphrase_f)();
};

void response_client_open(response_client_t *uclient, char *conn_str, char *(*id_f)(), char *(*pph_f)());
int response_client_send(response_client_t *uclient, response_request_t *req);
void response_client_recv(response_client_t *uclient);
void response_client_close(response_client_t *uclient);

void permission_unpack_socket(char *buff, perm_node_t **key_list, permission_table_t *permission_table, opool_t *opool);
void login_unpack_socket(char *buff, perm_node_t **key_list, login_table_t *login_table, opool_t *opool);
void asterisk_acc_unpack_socket(char *buff, perm_node_t **key_list, asterisk_acc_table_t *asterisk_acc_table, opool_t *opool);
void response_build_request(char *buffer, int len, response_request_t *request);
#endif