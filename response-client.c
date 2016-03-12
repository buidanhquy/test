#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#if 0
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#endif

#include "pjlib.h"

#include "json.h"
#include "response-client.h"
#include "my-pjlib-utils.h"
#include "ansi-utils.h"

#define UCLIENT_BUFSIZE 512

static void open_udp_socket(response_client_t *uclient, char *server, int port) {
    pj_sockaddr_in *saddr;
    pj_str_t s;
    // open socket
    CHECK(__FILE__, pj_sock_socket(PJ_AF_INET, PJ_SOCK_STREAM, 0, &uclient->fd));

    uclient->connect_data = malloc(sizeof(pj_sockaddr_in));
    saddr = (pj_sockaddr_in *)uclient->connect_data;
    pj_bzero((void *)saddr, sizeof(pj_sockaddr_in));
    saddr->sin_family = PJ_AF_INET;
    saddr->sin_port = pj_htons(port);
    saddr->sin_addr = pj_inet_addr(pj_cstr(&s, server));
    
    CHECK(__FILE__,pj_sock_connect(uclient->fd, (const pj_sockaddr_t *)saddr, sizeof(pj_sockaddr_in)));
}

static void open_tty(response_client_t *uclient, char *path) {
    (void) uclient;
    (void) path;
}

static void response_client_open_int(response_client_t *uclient, char *conn_str) {
    char *first, *second, *third;
    int len, port;

    memset(uclient->connect_str, 0, sizeof(uclient->connect_str));
    strncpy(uclient->connect_str, conn_str, strlen(conn_str));
    len = strlen(uclient->connect_str);
    uclient->connect_str[len] = '\0';

    first = conn_str;
    second = strchr(first, ':');
    EXIT_IF_TRUE(second == NULL, "Invalid connection string\n");
    *second = '\0';
    second++;
    if( 0 == strcmp(first, "udp") ) {
        third = strchr(second, ':');
        EXIT_IF_TRUE(third == NULL, "Invalid connection string for udp socket\n");
        *third = '\0';
        third++;
        port =  atoi(third);
        open_udp_socket(uclient, second, port);
    }
    else if( 0 == strcmp(first, "tty") ) {
        open_tty(uclient, second);
    }

}

void response_client_open(response_client_t *uclient, char *conn_str, char *(*id_f)(), char *(*pph_f)()) {
#if defined(USERVER_ENCRYPTED)
    uclient->id_f = id_f;
    uclient->passphrase_f = pph_f;
#else
    uclient->id_f = NULL;
    uclient->passphrase_f = NULL;
#endif
    //uclient->on_response_f = NULL;
    response_client_open_int(uclient, conn_str);
}
/*
void response_client_open_ex(response_client_t *uclient, char *conn_str, char *(*id_f)(), char *(*pph_f)()) {
    uclient->id_f = id_f;
    uclient->passphrase_f = pph_f;
    response_client_open_int(uclient, conn_str);
}
*/
int response_client_send(response_client_t *uclient, response_request_t *request) {
    int ret;
    long nbytes;
    char buff[UCLIENT_BUFSIZE];
#if defined(USERVER_ENCRYPTED)
    char cipher[UCLIENT_BUFSIZE];
    char message[UCLIENT_BUFSIZE];
    uint32_t timestamp;
    char sts[32];
    char *passphrase = uclient->passphrase_f();
    char otp[100];
    char *id = uclient->id_f();
    char challenge[32];
    int len = 32;
    int len1 = 32;
    
    lvc_t lvc;
    
    timestamp = get_ts();
    len1 = ts2str(timestamp, sts);
    
    generate_otp(otp, passphrase, sts);

    do_encrypt(challenge, &len, sts, len1, otp);

    lvc_init(&lvc, message, UCLIENT_BUFSIZE);

    fprintf(stdout, "lvc_pack id:%s\n", id);
    lvc_pack( &lvc, strlen(id), id );
    fprintf(stdout, "lvc_pack ts:%u\n", timestamp);
    lvc_pack( &lvc, sizeof(uint32_t), (char *)&timestamp );
    fprintf(stdout, "lvc_pack challenge:%d\n", len);
    lvc_pack( &lvc, len, challenge );
#endif
    response_build_request(buff, sizeof(buff), request);
    nbytes = strlen(buff);

#if defined(USERVER_ENCRYPTED)
    fprintf(stdout, "Message to send:%.*s\n", nbytes, buff);
    
    len = sizeof(cipher);
    do_encrypt(cipher, &len, buff, nbytes, otp);

    lvc_pack( &lvc, len, cipher );
    lvc_pack_finish(&lvc);
    
    nbytes = lvc.len;

    ret = pj_sock_sendto(uclient->fd, lvc.data, &nbytes, 0, (const pj_sockaddr_t *)uclient->connect_data, sizeof(pj_sockaddr_in));
#else
    ret = pj_sock_sendto(uclient->fd, buff, &nbytes, 0, (const pj_sockaddr_t *)uclient->connect_data, sizeof(pj_sockaddr_in));
#endif

    if(ret != 0) {
        PERROR_IF_TRUE(1, "Error in sending data\n");
        return -1;
    }

    return nbytes;
}
/*
int response_client_send(response_client_t *uclient, response_request_t *request) {
	int ret;
    long nbytes;
	char buff[UCLIENT_BUFSIZE];
	
	response_build_request(buff, sizeof(buff), request);
	
    nbytes = strlen(buff);
	ret = pj_sock_sendto(uclient->fd, buff, &nbytes, 0, (const pj_sockaddr_t *)uclient->connect_data, sizeof(pj_sockaddr_in));

    if(ret != 0) {
        PERROR_IF_TRUE(1, "Error in sending data\n");
        return -1;
    }

	return nbytes;
}
int response_client_send_ex(response_client_t *uclient, response_request_t *request) {
	int ret;
    long nbytes;
	char buff[UCLIENT_BUFSIZE];
    char cipher[UCLIENT_BUFSIZE];
    char message[UCLIENT_BUFSIZE];
    uint32_t timestamp;
    char sts[32];
    char *passphrase = uclient->passphrase_f();
    char otp[100];
    char *id = uclient->id_f();
    char challenge[32];
    int len = 32;
    int len1 = 32;
	
    lvc_t lvc;
    
    timestamp = get_ts();
    len1 = ts2str(timestamp, sts);
    
    generate_otp(otp, passphrase, sts);

    do_encrypt(challenge, &len, sts, len1, otp);

    lvc_init(&lvc, message, UCLIENT_BUFSIZE);

    fprintf(stdout, "lvc_pack id:%s\n", id);
    lvc_pack( &lvc, strlen(id), id );
    fprintf(stdout, "lvc_pack ts:%u\n", timestamp);
    lvc_pack( &lvc, sizeof(uint32_t), (char *)&timestamp );
    fprintf(stdout, "lvc_pack challenge:%d\n", len);
    lvc_pack( &lvc, len, challenge );

	response_build_request(buff, sizeof(buff), request);
    nbytes = strlen(buff);

    fprintf(stdout, "Message to send:%.*s\n", nbytes, buff);
    
    len = sizeof(cipher);
    do_encrypt(cipher, &len, buff, nbytes, otp);

    lvc_pack( &lvc, len, cipher );
    lvc_pack_finish(&lvc);
    
    nbytes = lvc.len;

	ret = pj_sock_sendto(uclient->fd, lvc.data, &nbytes, 0, (const pj_sockaddr_t *)uclient->connect_data, sizeof(pj_sockaddr_in));
	//ret = pj_sock_sendto(uclient->fd, buff, &nbytes, 0, (const pj_sockaddr_t *)uclient->connect_data, sizeof(pj_sockaddr_in));

    if(ret != 0) {
        PERROR_IF_TRUE(1, "Error in sending data\n");
        return -1;
    }

	return nbytes;
}
*/
void response_client_recv(response_client_t *uclient) {
    char buff[UCLIENT_BUFSIZE];
    long nbytes = UCLIENT_BUFSIZE;
    response_request_t req;
 
    int len = sizeof(pj_sockaddr_in);
  
    pj_sock_recvfrom(uclient->fd, (void *)buff, &nbytes, 0, (pj_sockaddr_t *)uclient->connect_data, &len);
    buff[nbytes] = '\0';
    printf("buf: %s\n", buff);

    response_parse_request(buff, sizeof(buff), &req);

#if 1
    if (uclient->on_response_f != NULL) {   
        uclient->on_response_f(uclient, &req);
    }
#endif
}
void response_client_close(response_client_t *uclient){
    pj_sock_close(uclient->fd);
    free(uclient->connect_data);
}

void permission_unpack_socket(char *buff, perm_node_t **key_list, permission_table_t *permission_table, opool_t *opool) {
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
#if 0
void login_unpack_socket(char *buff, perm_node_t **key_list, login_table_t *login_table, opool_t *opool) {
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

        json_object_object_get_ex(obj, "acc_id", &jvalue);
        strcpy(id, json_object_get_string(jvalue));

        json_object_object_get_ex(obj, "node_id", &jvalue);
        strcpy(pwd, json_object_get_string(jvalue));

        ansi_copy_str(key_element->value, id);
        ansi_copy_str(value_element->value, pwd);

        data_list = (perm_node_t *)ht_get_item(login_table, (void *)id);
        if (data_list == NULL) {
            DL_APPEND(*key_list, key_element);
            DL_APPEND(data_list, value_element);
            ht_add_item(login_table, id, data_list);
        }
        else {
            DL_APPEND(data_list, value_element);
        }
    }

}

void asterisk_acc_unpack_socket(char *buff, perm_node_t **key_list, asterisk_acc_table_t *asterisk_acc_table, opool_t *opool) {
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

        json_object_object_get_ex(obj, "account", &jvalue);
        strcpy(id, json_object_get_string(jvalue));

        json_object_object_get_ex(obj, "password", &jvalue);
        strcpy(pwd, json_object_get_string(jvalue));

        ansi_copy_str(key_element->value, id);
        ansi_copy_str(value_element->value, pwd);

        data_list = (perm_node_t *)ht_get_item(asterisk_acc_table, (void *)id);
        if (data_list == NULL) {
            DL_APPEND(*key_list, key_element);
            DL_APPEND(data_list, value_element);
            ht_add_item(asterisk_acc_table, id, data_list);
        }
        else {
            DL_APPEND(data_list, value_element);
        }
    }

}
#endif
void response_build_remove_per(json_object *jobj, response_request_t *request) {
    json_object_object_add(jobj, "status", json_object_new_string(request->remove_per.status));
    json_object_object_add(jobj, "err", json_object_new_int(request->remove_per.err));
    json_object_object_add(jobj, "data", json_object_new_string(request->remove_per.data));
}
void response_build_add_per(json_object *jobj, response_request_t *request) {
}
void response_build_list_acc(json_object *jobj, response_request_t *request) {
}
void response_build_list_per(json_object *jobj, response_request_t *request) {
}
void response_build_list_online_acc(json_object *jobj, response_request_t *request) {
}
void response_build_remove_acc(json_object *jobj, response_request_t *request) {
}
void response_build_add_acc(json_object *jobj, response_request_t *request) {
}
void response_build_request(char *buffer, int len, response_request_t *request) {
    json_object *jobj;
    jobj = json_object_new_object();
    json_object_object_add(jobj, "msg_id", json_object_new_int(request->msg_id));
    switch(request->msg_id) {
    case REMOVE_PER:
        response_build_remove_per(jobj, request);
        break;
    case ADD_PER:
        response_build_add_per(jobj, request);
        break;
    case LIST_ACC:
        response_build_list_acc(jobj, request);
        break;
    case LIST_PER:
        response_build_list_per(jobj, request);
        break;
    case LIST_ONLINE_ACC:
        response_build_list_online_acc(jobj, request);
        break;
    case REMOVE_ACC:
        response_build_remove_acc(jobj, request);
        break;
    case ADD_ACC:
        response_build_add_acc(jobj, request);
        break;
    default:
        fprintf(stderr, "response_build_request():Invalid msg_id: %d\n", request->msg_id);
        exit(-1);
    }
    strncpy(buffer, json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PLAIN), len);
    json_object_put(jobj);
}
