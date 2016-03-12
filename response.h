#ifndef __RESPONSE_H__
#define __RESPONSE_H__

typedef struct response_request_s response_request_t;
struct response_request_s {
    int msg_id;
    union {
        struct {
            int err;
            char data[512];
            char status[50];
        } list_acc;

        struct {
            int err;
            char data[512];
            char status[50];
        } list_online_acc;

        struct {
            int err;
            char data[512];
            char status[50];
        } list_per;

        struct {
            int err;
            char data[512];
            char status[50];
        } add_acc;

        struct {
            int err;
            char data[512];
            char status[50];
        } remove_acc;

        struct {
            int err;
            char data[512];
            char status[50];
        } add_per;

        struct {
            int err;
            char data[512];
            char status[50];
        } remove_per;

    };
};

#define REMOVE_PER 106
#define ADD_PER 105
#define LIST_ACC 100
#define LIST_PER 102
#define LIST_ONLINE_ACC 101
#define REMOVE_ACC 104
#define ADD_ACC 103
#endif
