#include "ansi-utils.h"

int main() {
    char str[] = "udp:192.168.2.50:1234";
    char port[4];
    extract_port(port, str);
    printf("port = %s\n", port);
    return 0;
}
