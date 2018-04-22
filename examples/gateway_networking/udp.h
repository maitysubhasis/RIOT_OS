#ifndef NET_MAIN_UDP_HDR_H
#define NET_MAIN_UDP_HDR_H

void send_ipv4(char *addr_str, char *port_str, char *data, kernel_pid_t iface_pid);

#endif