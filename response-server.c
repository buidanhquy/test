#include "json.h"
#include "response-server.h"
#include "lvcode.h"
#include "ansi-utils.h"
#include "my-pjlib-utils.h"

static void response_server_init_int(response_server_t *userver, char *conn_str, pj_pool_t *pool) {
    userver->pool = pool;
    ansi_copy_str(userver->connect_str, conn_str);
    userver->is_online = 1;
    CHECK(__FILE__, pj_mutex_create_simple(pool, "", &userver->mutex));
    if (userver->on_init_done_f != NULL){
        userver->on_init_done_f(userver);
	}
}

void response_server_init(response_server_t *userver, char *conn_str, pj_pool_t *pool, char *(*get_pph_f)(pj_str_t *id)) {
#if defined(USERVER_ENCRYPTED)
    userver->get_pph_f = get_pph_f;
#else
    userver->get_pph_f = NULL;
#endif
    response_server_init_int(userver, conn_str, pool);
}

static void open_udp_socket(response_server_t *userver, char *addr, int port) {
    pj_sockaddr_in saddr;
    pj_str_t s;
    int optval = 1;
    SHOW_LOG(3, __FILE__":open_udp_socket: %s:%d\n", addr, port);   
    // create udp socket here
    CHECK(__FILE__, pj_sock_socket(PJ_AF_INET, PJ_SOCK_STREAM, 0, &userver->fd));

#ifdef __ICS_INTEL__
    // Allow socket reuse
    //CHECK(__FILE__, pj_sock_setsockopt(userver->fd, PJ_SOL_SOCKET, 512, &optval, sizeof(optval)));
    CHECK(__FILE__, pj_sock_setsockopt(userver->fd, PJ_SOL_SOCKET, PJ_SO_REUSEADDR, &optval, sizeof(optval)));
#endif

    // bind the socket
    pj_bzero(&saddr, sizeof(saddr));
    saddr.sin_family = PJ_AF_INET;
    saddr.sin_port = pj_htons(port);
    saddr.sin_addr = pj_inet_addr(pj_cstr(&s,addr));

    CHECK(__FILE__, pj_sock_bind(userver->fd, &saddr, sizeof(saddr)));

    CHECK(__FILE__,pj_sock_listen(userver->fd, 5));
}

static void open_tty(response_server_t *userver, char *path) {
    (void)userver;
    (void)path;
#if 0
    struct termios options;
    
    userver->fd = open(path, O_RDWR | O_NOCTTY);
    EXIT_IF_TRUE(userver->fd < 0, "Cannot open port");
    fcntl(userver->fd, F_SETFL, 0);

    tcgetattr(userver->fd, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag |= (CREAD |CLOCAL);
    options.c_cflag &= ~CSIZE; /* Mask the character size bits */
    options.c_cflag |= CS8;    /* Select 8 data bits */
    options.c_cflag &= ~CRTSCTS;
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    //options->c_lflag |= (ICANON | ECHO | ECHOE); // Canonical mode

    options.c_lflag &= ~(ICANON | ECHO); // RAW mode
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 2; // measured in 0.1 second

    tcsetattr(userver->fd, TCSANOW, &options);
#endif
}

static int udp_recvfrom(int fd, char *buff, int len, void *data, unsigned int *data_len) {
    int ret;
    long nbytes = len;
    ret = pj_sock_recvfrom(fd, buff, &nbytes, 0, (pj_sockaddr_t *)data, (int *)data_len);
    if( ret != 0 ) {
        PERROR_IF_TRUE(1, "Error receiving data\n");
        return -1;
    }
    return nbytes;
}

static int tty_recvfrom(int fd, char *buff, int len, void *data, unsigned int *data_len) {
    (void)fd, (void) buff, (void) len, (void)data, (void)data_len;
    return -1;
    //return read(fd, buff, len);
}

static int udp_sendto(int fd, char *buff, int len, void *data, unsigned int data_len) {
    int ret = 0;
    long nbytes = len;
    ret = pj_sock_sendto(fd, buff, &nbytes, 0, (pj_sockaddr_t *)data, data_len);
    if( ret != 0 ) {
        PERROR_IF_TRUE(1, "Error in sending data\n");
        return -1;
    }
    return nbytes;
}

static int tty_sendto(int fd, char *buff, int len, void *data, unsigned int data_len) {
    (void)fd, (void) buff, (void) len, (void)data, (void)data_len;
    return -1;
    //return write(fd, buff, *len);
}

int response_unified_server_proc(void *param) {
    int ret;
    unsigned int len;
    
    int port;
    char *first, *second, *third;
    char cnt_str[30];

    pj_sockaddr_in caddr;
    char *caddr_str;
    pj_time_val timeout;

    pj_fd_set_t read_fds;

    char buffer[USERVER_BUFSIZE];
    response_request_t request;

#if defined(USERVER_ENCRYPTED)
    // For lvc parsing
    lvc_t lvc;
    int len1;
    char *val;
    uint32_t *ts;
    char sts[32];
    char plain[USERVER_BUFSIZE];
    char *pph;
    char otp[32];
    pj_str_t pjstr;
    // End for lvc parsing
#endif
    pj_sock_t newsock;

    response_server_t *userver = (response_server_t *)param;
    ansi_copy_str(cnt_str, userver->connect_str);

    first = cnt_str;
    
    second = strchr(first, ':');
    EXIT_IF_TRUE(second == NULL, "Wrong connection string format\n");
    *second = '\0';
    second++;

    if(0 == strcmp(first, "udp")) {
        third = strchr(second, ':');
        EXIT_IF_TRUE(third == NULL, "Wrong connection string format\n");
        *third = '\0';
        third++;
        port = atoi(third);
        open_udp_socket(userver, second, port);
        if (userver->on_open_socket_f != NULL)
            userver->on_open_socket_f(userver);

        userver->recv_f = &udp_recvfrom;
        userver->send_f = &udp_sendto;
    }
    else if( 0 == strcmp(first, "tty") ) {
        open_tty(userver, second);
        userver->recv_f = &tty_recvfrom;
        userver->send_f = &tty_sendto;
    }
    else {
        EXIT_IF_TRUE(1, "Unsuported protocol\n");
    }

    // thread loop
    timeout.sec = 0;
    timeout.msec = 100;
    userver->is_end = 0;

    while( !userver->is_end ) {

        while (!userver->is_online) {
            SHOW_LOG(3, "Server is currently offline...\n");
            pj_thread_sleep(1000);
        }

        PJ_FD_ZERO(&read_fds);
        PJ_FD_SET(userver->fd, &read_fds);

        pj_mutex_lock(userver->mutex);
        ret = pj_sock_select(userver->fd + 1, &read_fds, NULL, NULL, &timeout); 
        pj_mutex_unlock(userver->mutex);

        EXIT_IF_TRUE(ret < 0, "Error on server socket\n");

        if( PJ_FD_ISSET(userver->fd, &read_fds) ) {
            len = sizeof(caddr);
            pj_bzero(&caddr, len);

            pj_sock_accept(userver->fd, &newsock, &caddr, &len);

            pj_mutex_lock(userver->mutex);
            ret = userver->recv_f(newsock, buffer, USERVER_BUFSIZE, (void *)&caddr, &len);
            pj_mutex_unlock(userver->mutex);
            caddr_str = pj_inet_ntoa(caddr.sin_addr);

            if( ret > 0 ) {
                buffer[ret] = '\0';
#if defined(USERVER_ENCRYPTED)
                lvc_init(&lvc, buffer, ret);

                lvc_unpack(&lvc, &len, &val);
                pj_strset(&pjstr, val, len);
                pph = userver->get_pph_f(&pjstr);
                if( pph != NULL ) {

                    lvc_unpack(&lvc, &len, &val);
                    ts = (uint32_t *)val;
                    ts2str(*ts, sts);

                    lvc_unpack(&lvc, &len, &val);
                    
                    generate_otp(otp, pph, sts);
                    do_decrypt(val, len, plain, &len1, otp);
                    if( pj_ansi_strncmp(sts, plain, len1) == 0 ) {
                        lvc_unpack(&lvc, &len, &val);
                        do_decrypt(val, len, plain, &len1, otp);
                        plain[len1] = '\0';

                        response_parse_request(plain, len1, &request);
                        userver->on_request_f(userver, &request, &caddr, newsock);
                    }
                }
#else
                SHOW_LOG(1, "Received from client: %s\n", buffer);
                response_parse_request(buffer, ret, &request);
                userver->on_request_f(userver, &request, &caddr, newsock);
#endif
            }
        }
        pj_thread_sleep(100);
    }
    return 0;
}

void response_server_start(response_server_t *userver) {
    pj_thread_create(userver->pool, NULL, response_unified_server_proc, userver, PJ_THREAD_DEFAULT_STACK_SIZE, 0, &userver->master_thread);
    //pj_thread_create(userver->pool, NULL, response_server_proc, userver, PJ_THREAD_DEFAULT_STACK_SIZE, 0, &userver->master_thread);
}

/*
void response_server_start_ex(response_server_t *userver) {
    pj_thread_create(userver->pool, NULL, response_secure_server_proc, userver, PJ_THREAD_DEFAULT_STACK_SIZE, 0, &userver->master_thread);
}
*/

void response_server_join(response_server_t *userver, char *multicast_ip) {
    pj_ip_mreq mreq;
    pj_str_t s, s1;
    pj_status_t ret;

    pj_bzero(&mreq, sizeof(pj_ip_mreq));
	mreq.imr_multiaddr = pj_inet_addr(pj_cstr(&s, multicast_ip));
	mreq.imr_interface = pj_inet_addr(pj_cstr(&s1, "0.0.0.0"));

    //mreq.imr_multiaddr.s_addr = inet_addr(multicast_ip);
    //mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    pj_mutex_lock(userver->mutex);
    ret = pj_sock_setsockopt(userver->fd, PJ_SOL_IP, PJ_IP_ADD_MEMBERSHIP, &mreq,sizeof(mreq));
    PERROR_IF_TRUE(ret != 0, "Error in joining mcast group");
    pj_mutex_unlock(userver->mutex);
}

void response_server_leave(response_server_t *userver, char *multicast_ip) {
    pj_ip_mreq mreq;
    pj_str_t s;
    pj_status_t ret;
    mreq.imr_multiaddr = pj_inet_addr(pj_cstr(&s, multicast_ip));
    mreq.imr_interface.s_addr = pj_htonl(PJ_INADDR_ANY);

    pj_mutex_lock(userver->mutex);
    ret = pj_sock_setsockopt(userver->fd, PJ_SOL_IP, PJ_IP_DROP_MEMBERSHIP, &mreq,sizeof(mreq));
    PERROR_IF_TRUE(ret != 0, "Error in leaving mcast group");
    pj_mutex_unlock(userver->mutex);
}

void response_server_end(response_server_t *userver) {
    userver->is_end = 1;
    CHECK(__FILE__, pj_thread_join(userver->master_thread));
}

void permission_pack_socket(char *buff, perm_node_t *key_list, permission_table_t permission_table) {
    perm_node_t *key, *entry;
    perm_node_t *value, *entry2;
    perm_node_t *data_list;

    int i;

    json_object *my_array;
	my_array = json_object_new_array();

    DL_FOREACH_SAFE(key_list, key, entry) {
        data_list = (perm_node_t *)ht_get_item(&permission_table, (void *)key->value);
        if (data_list != NULL) {
            DL_FOREACH_SAFE(data_list, value, entry2) {
                json_object *obj;

                obj = json_object_new_object();
                json_object_object_add(obj, "acc_id", json_object_new_string(key->value));
                json_object_object_add(obj, "node_id", json_object_new_string(value->value));

                json_object_array_add(my_array, obj);
            }
        }
        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }
    }

    strcpy(buff, json_object_to_json_string(my_array));
}

void login_pack_socket(char *buff, perm_node_t *key_list, login_table_t login_table) {
    perm_node_t *key, *entry;
    perm_node_t *value, *entry2;
    perm_node_t *data_list;

    int i;

    json_object *my_array;
	my_array = json_object_new_array();

    DL_FOREACH_SAFE(key_list, key, entry) {
        data_list = (perm_node_t *)ht_get_item(&login_table, (void *)key->value);
        if (data_list != NULL) {
            DL_FOREACH_SAFE(data_list, value, entry2) {
                json_object *obj;

                obj = json_object_new_object();
                json_object_object_add(obj, "acc_id", json_object_new_string(key->value));
                json_object_object_add(obj, "node_id", json_object_new_string(value->value));

                json_object_array_add(my_array, obj);
            }
        }
        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }
    }

    strcpy(buff, json_object_to_json_string(my_array));
}

void asterisk_acc_pack_socket(char *buff, perm_node_t *key_list, asterisk_acc_table_t asterisk_acc_table) {
    perm_node_t *key, *entry;
    perm_node_t *value, *entry2;
    perm_node_t *data_list;

    int i;

    json_object *my_array;
	my_array = json_object_new_array();

    DL_FOREACH_SAFE(key_list, key, entry) {
        data_list = (perm_node_t *)ht_get_item(&asterisk_acc_table, (void *)key->value);
        if (data_list != NULL) {
            DL_FOREACH_SAFE(data_list, value, entry2) {
                json_object *obj;

                obj = json_object_new_object();
                json_object_object_add(obj, "account", json_object_new_string(key->value));
                json_object_object_add(obj, "password", json_object_new_string(value->value));

                json_object_array_add(my_array, obj);
            }
        }
        else {
            SHOW_LOG(3, "ht_get_item return null\n");
        }
    }

    strcpy(buff, json_object_to_json_string(my_array));
}
void response_parse_remove_per(json_object *jobj, char *buffer, int len, response_request_t *request) {
    json_object_object_foreach(jobj, key, val) {
        if(strcmp(key,"status") == 0) {
            strncpy(request->remove_per.status, json_object_get_string(val), sizeof(request->remove_per.status));
        }
        else if(strcmp(key,"err") == 0) {
            request->remove_per.err = json_object_get_int(val);
        }
        else if(strcmp(key,"data") == 0) {
            strncpy(request->remove_per.data, json_object_get_string(val), sizeof(request->remove_per.data));
        }
    }
}
void response_parse_add_per(json_object *jobj, char *buffer, int len, response_request_t *request) {
    json_object_object_foreach(jobj, key, val) {
    }
}
void response_parse_list_acc(json_object *jobj, char *buffer, int len, response_request_t *request) {
    json_object_object_foreach(jobj, key, val) {
    }
}
void response_parse_list_per(json_object *jobj, char *buffer, int len, response_request_t *request) {
    json_object_object_foreach(jobj, key, val) {
    }
}
void response_parse_list_online_acc(json_object *jobj, char *buffer, int len, response_request_t *request) {
    json_object_object_foreach(jobj, key, val) {
    }
}
void response_parse_remove_acc(json_object *jobj, char *buffer, int len, response_request_t *request) {
    json_object_object_foreach(jobj, key, val) {
    }
}
void response_parse_add_acc(json_object *jobj, char *buffer, int len, response_request_t *request) {
    json_object_object_foreach(jobj, key, val) {
    }
}
void response_parse_request(char *buffer, int len, response_request_t *request) {
    json_object *jobj, *jvalue;
    int msg_id;
    buffer[len] = '\0';
    jobj = json_tokener_parse(buffer);
    json_object_object_get_ex(jobj, "msg_id", &jvalue);
    msg_id = json_object_get_int(jvalue);
    request->msg_id = msg_id;
    switch(msg_id) {
    case REMOVE_PER:
        response_parse_remove_per(jobj, buffer, len, request);
        break;
    case ADD_PER:
        response_parse_add_per(jobj, buffer, len, request);
        break;
    case LIST_ACC:
        response_parse_list_acc(jobj, buffer, len, request);
        break;
    case LIST_PER:
        response_parse_list_per(jobj, buffer, len, request);
        break;
    case LIST_ONLINE_ACC:
        response_parse_list_online_acc(jobj, buffer, len, request);
        break;
    case REMOVE_ACC:
        response_parse_remove_acc(jobj, buffer, len, request);
        break;
    case ADD_ACC:
        response_parse_add_acc(jobj, buffer, len, request);
        break;
    default:
        fprintf(stderr, "response_parse_request(): Invalid msg_id; %d\n", msg_id);
        exit(-1);
    }
}
