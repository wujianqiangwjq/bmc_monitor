#include <ipmitool/ipmi.h>
#include <ipmitool/ipmi_intf.h>
#include <ipmitool/ipmi_main.h>
#include <ipmitool/ipmi_sdr.h>
#include <ipmitool/ipmi_gendev.h>
#include <ipmitool/ipmi_sel.h>
#include <ipmitool/ipmi_fru.h>
#include <ipmitool/ipmi_sol.h>
#include <ipmitool/ipmi_isol.h>
#include <ipmitool/ipmi_tsol.h>
#include <ipmitool/ipmi_lanp.h>
#include <ipmitool/ipmi_chassis.h>
#include <ipmitool/ipmi_mc.h>
#include <ipmitool/ipmi_sensor.h>
#include <ipmitool/ipmi_channel.h>
#include <ipmitool/ipmi_session.h>
#include <ipmitool/ipmi_event.h>
#include <ipmitool/ipmi_user.h>
#include <ipmitool/ipmi_raw.h>
#include <ipmitool/ipmi_pef.h>
#include <ipmitool/ipmi_oem.h>
#include <ipmitool/ipmi_sunoem.h>
#include <ipmitool/ipmi_fwum.h>
#include <ipmitool/ipmi_picmg.h>
#include <ipmitool/ipmi_kontronoem.h>
#include <ipmitool/ipmi_firewall.h>
#include <ipmitool/ipmi_hpmfwupg.h>
#include <ipmitool/ipmi_delloem.h>
#include <ipmitool/ipmi_ekanalyzer.h>
#include <ipmitool/ipmi_ime.h>
#include <ipmitool/ipmi_dcmi.h>
#include <ipmitool/ipmi_vita.h>
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef ENABLE_ALL_OPTIONS
# define OPTION_STRING  "I:46hVvcgsEKYao:H:d:P:f:U:p:C:L:A:t:T:m:z:S:l:b:B:e:k:y:O:R:N:D:"
#else
# define OPTION_STRING  "I:46hVvcH:f:U:p:d:S:D:"
#endif



int csv_output = 0;
int verbose = 0;
struct ipmi_cmd ipmitool_cmd_list[] = {
	{ ipmi_raw_main,     "raw",     "Send a RAW IPMI request and print response" },
	{ ipmi_rawi2c_main,  "i2c",     "Send an I2C Master Write-Read command and print response" },
	{ ipmi_rawspd_main,  "spd",     "Print SPD info from remote I2C device" },
	{ ipmi_lanp_main,    "lan",     "Configure LAN Channels" },
	{ ipmi_chassis_main, "chassis", "Get chassis status and set power state" },
	{ ipmi_power_main,   "power",   "Shortcut to chassis power commands" },
	{ ipmi_event_main,   "event",   "Send pre-defined events to MC" },
	{ ipmi_mc_main,      "mc",      "Management Controller status and global enables" },
	{ ipmi_mc_main,      "bmc",     NULL },
	{ ipmi_sensor_main,  "sensor",  "Print detailed sensor information" },
	{ ipmi_fru_main,     "fru",     "Print built-in FRU and scan SDR for FRU locators" },
	{ ipmi_gendev_main,  "gendev",  "Read/Write Device associated with Generic Device locators sdr" },
	{ ipmi_sel_main,     "sel",     "Print System Event Log (SEL)" },
	{ ipmi_pef_main,     "pef",     "Configure Platform Event Filtering (PEF)" },
	{ ipmi_sol_main,     "sol",     "Configure and connect IPMIv2.0 Serial-over-LAN" },
	{ ipmi_tsol_main,    "tsol",    "Configure and connect with Tyan IPMIv1.5 Serial-over-LAN" },
	{ ipmi_isol_main,    "isol",    "Configure IPMIv1.5 Serial-over-LAN" },
	{ ipmi_user_main,    "user",    "Configure Management Controller users" },
	{ ipmi_channel_main, "channel", "Configure Management Controller channels" },
	{ ipmi_session_main, "session", "Print session information" },
	{ ipmi_dcmi_main,    "dcmi",    "Data Center Management Interface"},
	{ ipmi_nm_main,      "nm",      "Node Manager Interface"},
	{ ipmi_sunoem_main,  "sunoem",  "OEM Commands for Sun servers" },
	{ ipmi_kontronoem_main, "kontronoem", "OEM Commands for Kontron devices"},
	{ ipmi_picmg_main,   "picmg",   "Run a PICMG/ATCA extended cmd"},
	{ ipmi_fwum_main,    "fwum",	"Update IPMC using Kontron OEM Firmware Update Manager" },
	{ ipmi_firewall_main,"firewall","Configure Firmware Firewall" },
	{ ipmi_delloem_main, "delloem", "OEM Commands for Dell systems" },
	{ ipmi_hpmfwupg_main,"hpm", "Update HPM components using PICMG HPM.1 file"},
	{ ipmi_ekanalyzer_main,"ekanalyzer", "run FRU-Ekeying analyzer using FRU files"},
	{ ipmi_ime_main,          "ime", "Update Intel Manageability Engine Firmware"},
	{ ipmi_vita_main,   "vita",   "Run a VITA 46.11 extended cmd"},
	{ NULL },
};




struct bmc_client*  bmc_connect(char* username, char *password, char*hostname){
     return ipmi_client(username,password,hostname,ipmitool_cmd_list,NULL);
}

void get_sdr(struct bmc_client* client, int num, char ** name){
    int rc;
    rc=ipmi_sdr_start_me(client->intf,client->sdr_list_itr);
    if (rc==0){
         ipmi_sdr_find_sdr_byids_me(client->intf,num, name,client->res,
		client->sdrr, client->sdr_list_itr);

    }   
   
}


int run_power_on(struct bmc_client* client){
    int rc =-1;
    uint8_t ctl = IPMI_CHASSIS_CTL_POWER_UP;
    rc = ipmi_chassis_power_control(client->intf, ctl);
	return rc;
}

int run_power_off(struct bmc_client* client){
    int rc = -1;
    uint8_t ctl = IPMI_CHASSIS_CTL_POWER_DOWN;
    rc = ipmi_chassis_power_control(client->intf, ctl);
	return rc;
}

int bmc_chassis_power_status(struct bmc_client* client){
    return ipmi_chassis_power_status(client->intf);
}

int bmc_close(struct bmc_client* client){
    ipmi_close(client);
}