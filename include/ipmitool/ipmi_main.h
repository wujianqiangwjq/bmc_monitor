/*
 * Copyright (c) 2003 Sun Microsystems, Inc.  All Rights Reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * Redistribution of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * Redistribution in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of Sun Microsystems, Inc. or the names of
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * 
 * This software is provided "AS IS," without a warranty of any kind.
 * ALL EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES,
 * INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED.
 * SUN MICROSYSTEMS, INC. ("SUN") AND ITS LICENSORS SHALL NOT BE LIABLE
 * FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING
 * OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.  IN NO EVENT WILL
 * SUN OR ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA,
 * OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR
 * PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF
 * LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE,
 * EVEN IF SUN HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef IPMI_MAIN_H
#define IPMI_MAIN_H

#include <ipmitool/ipmi_intf.h>
struct sdrData{
    int rc;
    char name[32];
    char value[16];
};
struct bmc_client{
    struct ipmi_intf* intf;
    struct ipmi_sdr_iterator* sdr_list_itr;
    struct sdr_record_list *sdrr;
    char *res;
};
int ipmi_main(int argc, char ** argv, struct ipmi_cmd * cmdlist, struct ipmi_intf_support * intflist);
void ipmi_cmd_print(struct ipmi_cmd * cmdlist);
int ipmi_cmd_run(struct ipmi_intf * intf, char * name, int argc, char ** argv);
struct bmc_client* 
ipmi_client(char* username, char *password, char*hostname,struct ipmi_cmd * cmdlist,struct ipmi_intf_support * intflist);
struct sdrData get_sdr_data(struct ipmi_intf* intf, char*  sdrname);
void ipmi_close(struct bmc_client * client);
#endif /* IPMI_MAIN_H */
