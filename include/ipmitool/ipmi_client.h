#ifndef IPMI_CLIENT_H
#define IPMI_CLIENT_H

#include <ipmitool/ipmi_intf.h>
struct ipmi_intf *
ipmi_client(char* username, char *password, char*hostname,
        struct ipmi_cmd * cmdlist,
        struct ipmi_intf_support * intflist);
#endif /* IPMI_MAIN_H */
