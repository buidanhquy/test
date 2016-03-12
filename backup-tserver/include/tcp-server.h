#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__
#include <pjlib.h>
#include "tcp.h"

#include "mysqlite.h"
#include <sqlite3.h>
#include "object-pool.h"
#include "utlist.h"
#include "proto-constants.h"
#include "permission.h"

#define USERVER_BUFSIZE 1024
typedef struct tcp_server_s tcp_server_t;

struct tcp_server_s {
    pj_sock_t fd;
    pj_pool_t *pool;
    pj_thread_t *master_thread;
    pj_mutex_t *mutex;
    char connect_str[30];
    volatile int is_end;
    int (*recv_f)(int fd, char *buff, int len, void *data, unsigned int *data_len);
    int (*send_f)(int fd, char *buff, int len, void *data, unsigned int data_len);
    void (*on_request_f)(tcp_server_t *userver, tcp_request_t *request, pj_sockaddr_in *caddr, pj_sock_t newsock);
    void (*on_init_done_f)(tcp_server_t *userver);
    void (*on_open_socket_f)(tcp_server_t *userver);
    void *user_data;

    int is_online;
};

void tcp_server_init(tcp_server_t *userver, char *conn_str, pj_pool_t *pool);
void tcp_server_start(tcp_server_t *userver);
void tcp_server_join(tcp_server_t *userver, char *multicast_ip);
void tcp_server_leave(tcp_server_t *userver, char *multicast_ip);
void tcp_server_end(tcp_server_t *userver);

int permission_pack_socket(char *buff, perm_node_t *key_list, permission_table_t permission_table);
int login_pack_socket(char *buff, perm_node_t *key_list, login_table_t login_table);
int asterisk_acc_pack_socket(char *buff, perm_node_t *key_list, asterisk_acc_table_t asterisk_acc_table);

void tcp_parse_request(char *buffer, int len, tcp_request_t *request);
void tcp_build_response(char *buffer, int len, tcp_response_t *response);
#endif
