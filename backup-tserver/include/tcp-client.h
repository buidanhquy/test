#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
#include <stdarg.h>
#include <pjlib.h>
#include "lvcode.h"
#include "tcp.h"

#include "mysqlite.h"
#include <sqlite3.h>
#include "object-pool.h"
#include "utlist.h"
#include "proto-constants.h"
#include "permission.h"

#define UCLIENT_BUFSIZE 1024

typedef struct tcp_client_s tcp_client_t;

struct tcp_client_s {
	pj_sock_t fd;
	char connect_str[30];
    void *connect_data;
    void (*on_response_f)(tcp_client_t *uclient, tcp_response_t *res);
    char *(*id_f)();
    char *(*passphrase_f)();
};

void tcp_client_open(tcp_client_t *uclient, char *conn_str, char *(*id_f)(), char *(*pph_f)());
int tcp_client_send(tcp_client_t *uclient, tcp_request_t *req);
void tcp_client_recv(tcp_client_t *uclient);
void tcp_client_close(tcp_client_t *uclient);

void permission_unpack_socket(char *buff, perm_node_t **key_list, permission_table_t *permission_table, opool_t *opool);
void login_unpack_socket(char *buff, perm_node_t **key_list, login_table_t *login_table, opool_t *opool);
void asterisk_acc_unpack_socket(char *buff, perm_node_t **key_list, asterisk_acc_table_t *asterisk_acc_table, opool_t *opool);
void tcp_build_request(char *buffer, int len, tcp_request_t *request);
void tcp_parse_response(char *buffer, int len, tcp_response_t *response);
#endif
