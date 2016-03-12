#ifndef __TCP_H__
#define __TCP_H__

typedef struct tcp_request_s tcp_request_t;
struct tcp_request_s {
    int msg_id;
    union {
        struct {
            char acc[50];
            char pwd[50];
        } list_acc;

        struct {
            char o_acc_id[50];
            char o_node_id[50];
        } list_online_acc;

        struct {
            char p_acc_id[50];
            char p_node_id[50];
        } list_per;

        struct {
            char a_acc[50];
            char a_pwd[50];
        } add_acc;

        struct {
            char r_acc[50];
        } remove_acc;

        struct {
            char a_acc_id[50];
            char a_node_id[50];
        } add_per;

        struct {
            char r_acc_id[50];
            char r_node_id[50];
        } remove_per;

    };
};


typedef struct tcp_response_s tcp_response_t;
struct tcp_response_s {
    int msg_id;
    int code;
    char data[1024];
    char status[50];
};
#define REMOVE_PER 106
#define ADD_PER 105
#define LIST_ACC 100
#define LIST_PER 102
#define LIST_ONLINE_ACC 101
#define REMOVE_ACC 104
#define ADD_ACC 103
#endif
