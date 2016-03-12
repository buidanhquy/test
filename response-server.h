#ifndef __RESPONSE_SERVER_H__
#define __RESPONSE_SERVER_H__
#include <pjlib.h>
#include "response.h"

#include "mysqlite.h"
#include <sqlite3.h>
#include "object-pool.h"
#include "utlist.h"
#include "proto-constants.h"
#include "permission.h"

#define USERVER_BUFSIZE 512
typedef struct response_server_s response_server_t;

struct response_server_s {
    pj_sock_t fd;
    pj_pool_t *pool;
    pj_thread_t *master_thread;
    pj_mutex_t *mutex;
    char connect_str[30];
    volatile int is_end;
    int (*recv_f)(int fd, char *buff, int len, void *data, unsigned int *data_len);
    int (*send_f)(int fd, char *buff, int len, void *data, unsigned int data_len);
    void (*on_request_f)(response_server_t *userver, response_request_t *request, pj_sockaddr_in *caddr, pj_sock_t newsock);
    void (*on_init_done_f)(response_server_t *userver);
    void (*on_open_socket_f)(response_server_t *userver);
    char *(*get_pph_f)(pj_str_t *id);
    void *user_data;

    int is_online;
};

void response_server_init(response_server_t *userver, char *conn_str, pj_pool_t *pool, char *(*get_pph_f)(pj_str_t *id));
void response_server_start(response_server_t *userver);
void response_server_join(response_server_t *userver, char *multicast_ip);
void response_server_leave(response_server_t *userver, char *multicast_ip);
void response_server_end(response_server_t *userver);

void permission_pack_socket(char *buff, perm_node_t *key_list, permission_table_t permission_table);
void login_pack_socket(char *buff, perm_node_t *key_list, login_table_t login_table);
void asterisk_acc_pack_socket(char *buff, perm_node_t *key_list, asterisk_acc_table_t asterisk_acc_table);
void response_parse_request(char *buffer, int len, response_request_t *request);
#endif