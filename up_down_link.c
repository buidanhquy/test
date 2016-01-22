#include <asm/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <netinet/in.h>
 
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

int main(){
        int fd;
        struct sockaddr_nl sa;
        struct ifaddrmsg *rtmp;
        struct rtattr *rtatp;
        int rtattrlen;
        struct ifinfomsg *rtif;
        char ifname[IF_NAMESIZE];
        int len;
        char buf[4096];
        struct iovec iov = { buf, sizeof(buf) };
        struct nlmsghdr *nh;

        memset(&sa, 0, sizeof(sa));
        sa.nl_family = AF_NETLINK;
        sa.nl_groups = RTMGRP_LINK;

        fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
        bind(fd, (struct sockaddr *) &sa, sizeof(sa));

        while(1){
                struct msghdr msg = { &sa, sizeof(sa), &iov, 1, NULL, 0, 0 };
                len = recvmsg(fd, &msg, 0);

                for (nh = (struct nlmsghdr *) buf; NLMSG_OK (nh, len); nh = NLMSG_NEXT (nh, len)) {
                        /* The end of multipart message. */
                        if (nh->nlmsg_type == NLMSG_DONE){
                                printf("got msg done\n");
                                break;
                        }

                        if (nh->nlmsg_type == NLMSG_ERROR){
                                printf("got msg error\n");
                                continue;
                        }
                        if (nh->nlmsg_type < RTM_NEWADDR){
                                rtif=(struct ifinfomsg *)NLMSG_DATA(nh);
                                printf("Interface %s is %s\n",if_indextoname(rtif->ifi_index,ifname), 
                                         (rtif->ifi_flags&IFF_RUNNING)?"Connected":"Disconnected");
                        }
                }
        }
        return 0;
}
