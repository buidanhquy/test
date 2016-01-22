
#include "gb-sender.h"

int main () {
    gb_sender_t gb_sender;
    gb_sender_create(&gb_sender, "udp:238.0.0.1:2016");

    while (1) {
        gb_sender_report_online(gb_sender, "SENDER_1", "number 1", 1, 1);
        usleep(5*1000*1000);
    }
    return 0;
}
