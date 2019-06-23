#include <ipmitool/ipmi_main.h>
/* From src/plugins/ipmi_intf.c: */
void
ipmi_intf_set_max_request_data_size_sdr(struct ipmi_intf * intf, uint16_t size);

extern int verbose;
extern int csv_output;
extern const struct valstr ipmi_privlvl_vals[];
extern const struct valstr ipmi_authtype_session_vals[];
char* VERSION="1.8.18";
struct ipmi_intf * ipmi_main_intf_sdr = NULL;

/* ipmi_password_file_read  -  Open file and read password from it
 *
 * @filename:	file name to read from
 *
 * returns pointer to allocated buffer containing password
 *   (caller is expected to free when finished)
 * returns NULL on error
 */
ipmi_password_file_read(char * filename)
{
	FILE * fp;
	char * pass = NULL;
	int l;

	pass = malloc(21);
	if (pass == NULL) {
		lprintf(LOG_ERR, "ipmitool: malloc failure");
		return NULL;
	}

	memset(pass, 0, 21);
	fp = ipmi_open_file_read((const char *)filename);
	if (fp == NULL) {
		lprintf(LOG_ERR, "Unable to open password file %s",
				filename);
		free(pass);
		return NULL;
	}

	/* read in id */
	if (fgets(pass, 21, fp) == NULL) {
		lprintf(LOG_ERR, "Unable to read password from file %s",
				filename);
		free(pass);
		fclose(fp);
		return NULL;
	}

        /* remove traling <cr><nl><tab> */
	l = strcspn(pass, "\r\n\t");
	if (l > 0) {
		pass[l] = '\0';
	}

	fclose(fp);
	return pass;
}


/*
 * Print all the commands in the above table to stderr
 * used for help text on command line and shell
 */
void
ipmi_cmd_print_sdr(struct ipmi_cmd * cmdlist)
{
	struct ipmi_cmd * cmd;
	int hdr = 0;

	if (cmdlist == NULL)
		return;
	for (cmd=cmdlist; cmd->func != NULL; cmd++) {
		if (cmd->desc == NULL)
			continue;
		if (hdr == 0) {
			lprintf(LOG_NOTICE, "Commands:");
			hdr = 1;
		}
		lprintf(LOG_NOTICE, "\t%-12s  %s", cmd->name, cmd->desc);
	}
	lprintf(LOG_NOTICE, "");
}



static void
ipmi_option_usage(const char * progname, struct ipmi_cmd * cmdlist, struct ipmi_intf_support * intflist)
{
	lprintf(LOG_NOTICE, "%s version %s\n", progname, VERSION);
	lprintf(LOG_NOTICE, "usage: %s [options...] <command>\n", progname);
	lprintf(LOG_NOTICE, "       -h             This help");
	lprintf(LOG_NOTICE, "       -V             Show version information");
	lprintf(LOG_NOTICE, "       -v             Verbose (can use multiple times)");
	lprintf(LOG_NOTICE, "       -c             Display output in comma separated format");
	lprintf(LOG_NOTICE, "       -d N           Specify a /dev/ipmiN device to use (default=0)");
	lprintf(LOG_NOTICE, "       -I intf        Interface to use");
	lprintf(LOG_NOTICE, "       -H hostname    Remote host name for LAN interface");
	lprintf(LOG_NOTICE, "       -p port        Remote RMCP port [default=623]");
	lprintf(LOG_NOTICE, "       -U username    Remote session username");
	lprintf(LOG_NOTICE, "       -f file        Read remote session password from file");
	lprintf(LOG_NOTICE, "       -z size        Change Size of Communication Channel (OEM)");
	lprintf(LOG_NOTICE, "       -S sdr         Use local file for remote SDR cache");
	lprintf(LOG_NOTICE, "       -D tty:b[:s]   Specify the serial device, baud rate to use");
	lprintf(LOG_NOTICE, "                      and, optionally, specify that interface is the system one");
	lprintf(LOG_NOTICE, "       -4             Use only IPv4");
	lprintf(LOG_NOTICE, "       -6             Use only IPv6");
#ifdef ENABLE_ALL_OPTIONS
	lprintf(LOG_NOTICE, "       -a             Prompt for remote password");
	lprintf(LOG_NOTICE, "       -Y             Prompt for the Kg key for IPMIv2 authentication");
	lprintf(LOG_NOTICE, "       -e char        Set SOL escape character");
	lprintf(LOG_NOTICE, "       -C ciphersuite Cipher suite to be used by lanplus interface");
	lprintf(LOG_NOTICE, "       -k key         Use Kg key for IPMIv2 authentication");
	lprintf(LOG_NOTICE, "       -y hex_key     Use hexadecimal-encoded Kg key for IPMIv2 authentication");
	lprintf(LOG_NOTICE, "       -L level       Remote session privilege level [default=ADMINISTRATOR]");
	lprintf(LOG_NOTICE, "                      Append a '+' to use name/privilege lookup in RAKP1");
	lprintf(LOG_NOTICE, "       -A authtype    Force use of auth type NONE, PASSWORD, MD2, MD5 or OEM");
	lprintf(LOG_NOTICE, "       -P password    Remote session password");
	lprintf(LOG_NOTICE, "       -E             Read password from IPMI_PASSWORD environment variable");
	lprintf(LOG_NOTICE, "       -K             Read kgkey from IPMI_KGKEY environment variable");
	lprintf(LOG_NOTICE, "       -m address     Set local IPMB address");
	lprintf(LOG_NOTICE, "       -b channel     Set destination channel for bridged request");
	lprintf(LOG_NOTICE, "       -t address     Bridge request to remote target address");
	lprintf(LOG_NOTICE, "       -B channel     Set transit channel for bridged request (dual bridge)");
	lprintf(LOG_NOTICE, "       -T address     Set transit address for bridge request (dual bridge)");
	lprintf(LOG_NOTICE, "       -l lun         Set destination lun for raw commands");
	lprintf(LOG_NOTICE, "       -o oemtype     Setup for OEM (use 'list' to see available OEM types)");
	lprintf(LOG_NOTICE, "       -O seloem      Use file for OEM SEL event descriptions");
	lprintf(LOG_NOTICE, "       -N seconds     Specify timeout for lan [default=2] / lanplus [default=1] interface");
	lprintf(LOG_NOTICE, "       -R retry       Set the number of retries for lan/lanplus interface [default=4]");
#endif
	lprintf(LOG_NOTICE, "");

	ipmi_intf_print(intflist);

	if (cmdlist != NULL)
		ipmi_cmd_print_sdr(cmdlist);
}
/* ipmi_catch_sigint_sdr  -  Handle the interrupt signal (Ctrl-C), close the
 *                       interface, and exit ipmitool with error (-1)
 *
 *                       This insures that the IOL session gets freed
 *                       for other callers.
 *
 * returns -1
 */
void ipmi_catch_sigint_sdr()
{
	if (ipmi_main_intf_sdr != NULL) {
		printf("\nSIGN INT: Close Interface %s\n",ipmi_main_intf_sdr->desc);
		/* reduce retry count to a single retry */
		ipmi_main_intf_sdr->ssn_params.retry = 1;
		/* close interface */
		ipmi_main_intf_sdr->close(ipmi_main_intf_sdr);
	}
	exit(-1);
}

static uint8_t
ipmi_acquire_ipmb_address(struct ipmi_intf * intf)
{
	if (intf->picmg_avail) {
		return ipmi_picmg_ipmb_address(intf);
	} else if (intf->vita_avail) {
		return ipmi_vita_ipmb_address(intf);
	} else {
		return 0;
    }
}

/* ipmi_parse_options  -  helper function to handle parsing command line options
 *
 * @argc:	count of options
 * @argv:	list of options
 * @cmdlist:	list of supported commands
 * @intflist:	list of supported interfaces
 *
 * returns 0 on success
 * returns -1 on error
 */
struct ipmi_intf * get_intf(){
	 return ipmi_main_intf_sdr;
}


int
ipmi_main_sdr(int argc, char ** argv,
		struct ipmi_cmd * cmdlist,
		struct ipmi_intf_support * intflist)
{
	struct ipmi_intf_support * sup;
	int privlvl = 0;
	uint8_t target_addr = 0;
	uint8_t target_channel = 0;

	uint8_t transit_addr = 0;
	uint8_t transit_channel = 0;
	uint8_t target_lun     = 0;
	uint8_t arg_addr = 0;
	uint8_t addr = 0;
	uint16_t my_long_packet_size=0;
	uint8_t my_long_packet_set=0;
	uint8_t lookupbit = 0x10;	/* use name-only lookup by default */
	int retry = 0;
	uint32_t timeout = 0;
	int authtype = -1;
	char * tmp_pass = NULL;
	char * tmp_env = NULL;
	char * hostname = NULL;
	char * username = NULL;
	char * password = NULL;
	char * intfname = NULL;
	char * progname = NULL;
	char * oemtype  = NULL;
	char * sdrcache = NULL;
	uint8_t kgkey[IPMI_KG_BUFFER_SIZE];
	char * seloem   = NULL;
	int port = 0;
	int devnum = 0;
	int cipher_suite_id = 3; /* See table 22-19 of the IPMIv2 spec */
	int argflag, i, found;
	int rc = -1;
	int ai_family = AF_UNSPEC;
	char sol_escape_char = SOL_ESCAPE_CHARACTER_DEFAULT;
	char * devfile  = NULL;

	/* save program name */
	progname = strrchr(argv[0], '/');
	progname = ((progname == NULL) ? argv[0] : progname+1);
	signal(SIGINT, ipmi_catch_sigint_sdr);
	memset(kgkey, 0, sizeof(kgkey));

	while ((argflag = getopt(argc, (char **)argv, OPTION_STRING)) != -1)
	{
		switch (argflag) {
		case 'I':
			if (intfname) {
				free(intfname);
				intfname = NULL;
			}
			intfname = strdup(optarg);
			if (intfname == NULL) {
				lprintf(LOG_ERR, "%s: malloc failure", progname);
				goto out_free;
			}
			if (intflist != NULL) {
				found = 0;
				for (sup=intflist; sup->name != NULL; sup++) {
					if (strncmp(sup->name, intfname, strlen(intfname)) == 0 &&
							strncmp(sup->name, intfname, strlen(sup->name)) == 0 &&
							sup->supported == 1)
						found = 1;
				}
				if (!found) {
					lprintf(LOG_ERR, "Interface %s not supported", intfname);
					goto out_free;
				}
			}
			break;
		case 'h':
			ipmi_option_usage(progname, cmdlist, intflist);
			rc = 0;
			goto out_free;
			break;
		case 'V':
			printf("%s version %s\n", progname, VERSION);
			rc = 0;
			goto out_free;
			break;
		case 'd':
			if (str2int(optarg, &devnum) != 0) {
				lprintf(LOG_ERR, "Invalid parameter given or out of range for '-d'.");
				rc = -1;
				goto out_free;
			}
			/* Check if device number is -gt 0; I couldn't find limit for
			 * kernels > 2.6, thus right side is unlimited.
			 */
			if (devnum < 0) {
				lprintf(LOG_ERR, "Device number %i is out of range.", devnum);
				rc = -1;
				goto out_free;
			}
			break;
		case 'p':
			if (str2int(optarg, &port) != 0) {
				lprintf(LOG_ERR, "Invalid parameter given or out of range for '-p'.");
				rc = -1;
				goto out_free;
			}
			/* Check if port is -gt 0 && port is -lt 65535 */
			if (port < 0 || port > 65535) {
				lprintf(LOG_ERR, "Port number %i is out of range.", port);
				rc = -1;
				goto out_free;
			}
			break;
		case 'C':
			if (str2int(optarg, &cipher_suite_id) != 0) {
				lprintf(LOG_ERR, "Invalid parameter given or out of range for '-C'.");
				rc = -1;
				goto out_free;
			}
			/* add check Cipher is -gt 0 */
			if (cipher_suite_id < 0) {
				lprintf(LOG_ERR, "Cipher suite ID %i is invalid.", cipher_suite_id);
				rc = -1;
				goto out_free;
			}
			break;
		case 'v':
			verbose++;
			break;
		case 'c':
			csv_output = 1;
			break;
		case 'H':
			if (hostname) {
				free(hostname);
				hostname = NULL;
			}
			hostname = strdup(optarg);
			if (hostname == NULL) {
				lprintf(LOG_ERR, "%s: malloc failure", progname);
				goto out_free;
			}
			break;
		case 'f':
			if (password) {
				free(password);
				password = NULL;
			}
			password = ipmi_password_file_read(optarg);
			if (password == NULL)
				lprintf(LOG_ERR, "Unable to read password "
						"from file %s", optarg);
			break;
		case 'a':
#ifdef HAVE_GETPASSPHRASE
			tmp_pass = getpassphrase("Password: ");
#else
			tmp_pass = getpass("Password: ");
#endif
			if (tmp_pass != NULL) {
				if (password) {
					free(password);
					password = NULL;
				}
				password = strdup(tmp_pass);
				tmp_pass = NULL;
				if (password == NULL) {
					lprintf(LOG_ERR, "%s: malloc failure", progname);
					goto out_free;
				}
			}
			break;
		case 'k':
			memset(kgkey, 0, sizeof(kgkey));
			strncpy((char *)kgkey, optarg, sizeof(kgkey) - 1);
			break;
		case 'K':
			if ((tmp_env = getenv("IPMI_KGKEY"))) {
				memset(kgkey, 0, sizeof(kgkey));
				strncpy((char *)kgkey, tmp_env,
					sizeof(kgkey) - 1);
			} else {
				lprintf(LOG_WARN, "Unable to read kgkey from environment");
			}
			break;
		case 'y':
			memset(kgkey, 0, sizeof(kgkey));

			rc = ipmi_parse_hex(optarg, kgkey, sizeof(kgkey) - 1);
			if (rc == -1) {
				lprintf(LOG_ERR, "Number of Kg key characters is not even");
				goto out_free;
			} else if (rc == -3) {
				lprintf(LOG_ERR, "Kg key is not hexadecimal number");
				goto out_free;
			} else if (rc > (IPMI_KG_BUFFER_SIZE-1)) {
				lprintf(LOG_ERR, "Kg key is too long");
				goto out_free;
			}
			break;
		case 'Y':
#ifdef HAVE_GETPASSPHRASE
			tmp_pass = getpassphrase("Key: ");
#else
			tmp_pass = getpass("Key: ");
#endif
			if (tmp_pass != NULL) {
				memset(kgkey, 0, sizeof(kgkey));
				strncpy((char *)kgkey, tmp_pass,
					sizeof(kgkey) - 1);
				tmp_pass = NULL;
			}
			break;
		case 'U':
			if (username) {
				free(username);
				username = NULL;
			}
			if (strlen(optarg) > 16) {
				lprintf(LOG_ERR, "Username is too long (> 16 bytes)");
				goto out_free;
			}
			username = strdup(optarg);
			if (username == NULL) {
				lprintf(LOG_ERR, "%s: malloc failure", progname);
				goto out_free;
			}
			break;
		case 'S':
			if (sdrcache) {
				free(sdrcache);
				sdrcache = NULL;
			}
			sdrcache = strdup(optarg);
			if (sdrcache == NULL) {
				lprintf(LOG_ERR, "%s: malloc failure", progname);
				goto out_free;
			}
			break;
		case 'D':
			/* check for subsequent instance of -D */
			if (devfile) {
				/* free memory for previous string */
				free(devfile);
			}
			devfile = strdup(optarg);
			if (devfile == NULL) {
				lprintf(LOG_ERR, "%s: malloc failure", progname);
				goto out_free;
			}
			break;
		case '4':
			/* IPv4 only */
			if (ai_family == AF_UNSPEC) {
				ai_family = AF_INET;
			} else {
				if (ai_family == AF_INET6) {
					lprintf(LOG_ERR,
						"Parameter is mutually exclusive with -6.");
				} else {
					lprintf(LOG_ERR,
						"Multiple -4 parameters given.");
				}
				rc = (-1);
				goto out_free;
			}
			break;
		case '6':
			/* IPv6 only */
			if (ai_family == AF_UNSPEC) {
				ai_family = AF_INET6;
			} else {
				if (ai_family == AF_INET) {
					lprintf(LOG_ERR,
						"Parameter is mutually exclusive with -4.");
				} else {
					lprintf(LOG_ERR,
						"Multiple -6 parameters given.");
				}
				rc = (-1);
				goto out_free;
			}
			break;
#ifdef ENABLE_ALL_OPTIONS
		case 'o':
			if (oemtype) {
				free(oemtype);
				oemtype = NULL;
			}
			oemtype = strdup(optarg);
			if (oemtype == NULL) {
				lprintf(LOG_ERR, "%s: malloc failure", progname);
				goto out_free;
			}
			if (strncmp(oemtype, "list", 4) == 0 ||
					strncmp(oemtype, "help", 4) == 0) {
				ipmi_oem_print();
				rc = 0;
				goto out_free;
			}
			break;
		case 'g':
			/* backwards compatible oem hack */
			if (oemtype) {
				free(oemtype);
				oemtype = NULL;
			}
			oemtype = strdup("intelwv2");
			break;
		case 's':
			/* backwards compatible oem hack */
			if (oemtype) {
				free(oemtype);
				oemtype = NULL;
			}
			oemtype = strdup("supermicro");
			break;
		case 'P':
			if (password) {
				free(password);
				password = NULL;
			}
			password = strdup(optarg);
			if (password == NULL) {
				lprintf(LOG_ERR, "%s: malloc failure", progname);
				goto out_free;
			}

			/* Prevent password snooping with ps */
			i = strlen(optarg);
			memset(optarg, 'X', i);
			break;
		case 'E':
			if ((tmp_env = getenv("IPMITOOL_PASSWORD"))) {
				if (password) {
					free(password);
					password = NULL;
				}
				password = strdup(tmp_env);
				if (password == NULL) {
					lprintf(LOG_ERR, "%s: malloc failure", progname);
					goto out_free;
				}
			}
			else if ((tmp_env = getenv("IPMI_PASSWORD"))) {
				if (password) {
					free(password);
					password = NULL;
				}
				password = strdup(tmp_env);
				if (password == NULL) {
					lprintf(LOG_ERR, "%s: malloc failure", progname);
					goto out_free;
				}
			}
			else {
				lprintf(LOG_WARN, "Unable to read password from environment");
			}
			break;
		case 'L':
			i = strlen(optarg);
			if ((i > 0) && (optarg[i-1] == '+')) {
				lookupbit = 0;
				optarg[i-1] = 0;
			}
			privlvl = str2val(optarg, ipmi_privlvl_vals);
			if (privlvl == 0xFF) {
				lprintf(LOG_WARN, "Invalid privilege level %s", optarg);
			}
			break;
		case 'A':
			authtype = str2val(optarg, ipmi_authtype_session_vals);
			break;
		case 't':
			if (str2uchar(optarg, &target_addr) != 0) {
				lprintf(LOG_ERR, "Invalid parameter given or out of range for '-t'.");
				rc = -1;
				goto out_free;
			}
			break;
		case 'b':
			if (str2uchar(optarg, &target_channel) != 0) {
				lprintf(LOG_ERR, "Invalid parameter given or out of range for '-b'.");
				rc = -1;
				goto out_free;
			}
			break;
		case 'T':
			if (str2uchar(optarg, &transit_addr) != 0) {
				lprintf(LOG_ERR, "Invalid parameter given or out of range for '-T'.");
				rc = -1;
				goto out_free;
			}
			break;
		case 'B':
			if (str2uchar(optarg, &transit_channel) != 0) {
				lprintf(LOG_ERR, "Invalid parameter given or out of range for '-B'.");
				rc = -1;
				goto out_free;
			}
			break;
		case 'l':
			if (str2uchar(optarg, &target_lun) != 0) {
				lprintf(LOG_ERR, "Invalid parameter given or out of range for '-l'.");
				rc = 1;
				goto out_free;
			}
			break;
		case 'm':
			if (str2uchar(optarg, &arg_addr) != 0) {
				lprintf(LOG_ERR, "Invalid parameter given or out of range for '-m'.");
				rc = -1;
				goto out_free;
			}
			break;
		case 'e':
			sol_escape_char = optarg[0];
			break;
		case 'O':
			if (seloem) {
				free(seloem);
				seloem = NULL;
			}
			seloem = strdup(optarg);
			if (seloem == NULL) {
				lprintf(LOG_ERR, "%s: malloc failure", progname);
				goto out_free;
			}
			break;
		case 'z':
			if (str2ushort(optarg, &my_long_packet_size) != 0) {
				lprintf(LOG_ERR, "Invalid parameter given or out of range for '-z'.");
				rc = -1;
				goto out_free;
			}
			break;
		/* Retry and Timeout */
		case 'R':
			if (str2int(optarg, &retry) != 0 || retry < 0) {
				lprintf(LOG_ERR, "Invalid parameter given or out of range for '-R'.");
				rc = -1;
				goto out_free;
			}
			break;
		case 'N':
			if (str2uint(optarg, &timeout) != 0) {
				lprintf(LOG_ERR, "Invalid parameter given or out of range for '-N'.");
				rc = -1;
				goto out_free;
			}
			break;
#endif
		default:
			ipmi_option_usage(progname, cmdlist, intflist);
			goto out_free;
		}
	}

	/* check for command before doing anything */
	if (argc-optind > 0 &&
			strncmp(argv[optind], "help", 4) == 0) {
		ipmi_cmd_print_sdr(cmdlist);
		rc = 0;
		goto out_free;
	}

	/*
	 * If the user has specified a hostname (-H option)
	 * then this is a remote access session.
	 *
	 * If no password was specified by any other method
	 * and the authtype was not explicitly set to NONE
	 * then prompt the user.
	 */
	if (hostname != NULL && password == NULL &&
			(authtype != IPMI_SESSION_AUTHTYPE_NONE || authtype < 0)) {
#ifdef HAVE_GETPASSPHRASE
		tmp_pass = getpassphrase("Password: ");
#else
		tmp_pass = getpass("Password: ");
#endif
		if (tmp_pass != NULL) {
			password = strdup(tmp_pass);
			tmp_pass = NULL;
			if (password == NULL) {
				lprintf(LOG_ERR, "%s: malloc failure", progname);
				goto out_free;
			}
		}
	}

	/* if no interface was specified but a
	 * hostname was then use LAN by default
	 * otherwise the default is hardcoded
	 * to use the first entry in the list
	 */
	if (intfname == NULL && hostname != NULL) {
		intfname = strdup("lan");
		if (intfname == NULL) {
			lprintf(LOG_ERR, "%s: malloc failure", progname);
			goto out_free;
		}
	}

	if (password != NULL && intfname != NULL) {
		if (strcmp(intfname, "lan") == 0 && strlen(password) > 16) {
			lprintf(LOG_ERR, "%s: password is longer than 16 bytes.", intfname);
			rc = -1;
			goto out_free;
		} else if (strcmp(intfname, "lanplus") == 0 && strlen(password) > 20) {
			lprintf(LOG_ERR, "%s: password is longer than 20 bytes.", intfname);
			rc = -1;
			goto out_free;
		}
	} /* if (password != NULL && intfname != NULL) */

	/* load interface */
	ipmi_main_intf_sdr = ipmi_intf_load(intfname);
	if (ipmi_main_intf_sdr == NULL) {
		lprintf(LOG_ERR, "Error loading interface %s", intfname);
		goto out_free;
	}

	/* setup log */
	log_init(progname, 0, verbose);

	/* run OEM setup if found */
	if (oemtype != NULL &&
	    ipmi_oem_setup(ipmi_main_intf_sdr, oemtype) < 0) {
		lprintf(LOG_ERR, "OEM setup for \"%s\" failed", oemtype);
		goto out_free;
	}

	/* set session variables */
	if (hostname != NULL)
		ipmi_intf_session_set_hostname(ipmi_main_intf_sdr, hostname);
	if (username != NULL)
		ipmi_intf_session_set_username(ipmi_main_intf_sdr, username);
	if (password != NULL)
		ipmi_intf_session_set_password(ipmi_main_intf_sdr, password);
	ipmi_intf_session_set_kgkey(ipmi_main_intf_sdr, kgkey);
	if (port > 0)
		ipmi_intf_session_set_port(ipmi_main_intf_sdr, port);
	if (authtype >= 0)
		ipmi_intf_session_set_authtype(ipmi_main_intf_sdr, (uint8_t)authtype);
	if (privlvl > 0)
		ipmi_intf_session_set_privlvl(ipmi_main_intf_sdr, (uint8_t)privlvl);
	else
		ipmi_intf_session_set_privlvl(ipmi_main_intf_sdr,
				IPMI_SESSION_PRIV_ADMIN);	/* default */
	/* Adding retry and timeout for interface that support it */
	if (retry > 0)
		ipmi_intf_session_set_retry(ipmi_main_intf_sdr, retry);
	if (timeout > 0)
		ipmi_intf_session_set_timeout(ipmi_main_intf_sdr, timeout);

	ipmi_intf_session_set_lookupbit(ipmi_main_intf_sdr, lookupbit);
	ipmi_intf_session_set_sol_escape_char(ipmi_main_intf_sdr, sol_escape_char);
	ipmi_intf_session_set_cipher_suite_id(ipmi_main_intf_sdr, cipher_suite_id);

	ipmi_main_intf_sdr->devnum = devnum;

	/* setup device file if given */
	ipmi_main_intf_sdr->devfile = devfile;

	ipmi_main_intf_sdr->ai_family = ai_family;
	/* Open the interface with the specified or default IPMB address */
	ipmi_main_intf_sdr->my_addr = arg_addr ? arg_addr : IPMI_BMC_SLAVE_ADDR;
	if (ipmi_main_intf_sdr->open != NULL) {
		if (ipmi_main_intf_sdr->open(ipmi_main_intf_sdr) < 0) {
			goto out_free;
		}
	}

	if (!ipmi_oem_active(ipmi_main_intf_sdr, "i82571spt")) {
		/*
		 * Attempt picmg/vita discovery of the actual interface
		 * address, unless the users specified an address.
		 * Address specification always overrides discovery
		 */
		if (picmg_discover(ipmi_main_intf_sdr)) {
			ipmi_main_intf_sdr->picmg_avail = 1;
		} else if (vita_discover(ipmi_main_intf_sdr)) {
			ipmi_main_intf_sdr->vita_avail = 1;
		}
	}

	if (arg_addr) {
		addr = arg_addr;
	} else if (!ipmi_oem_active(ipmi_main_intf_sdr, "i82571spt")) {
		lprintf(LOG_DEBUG, "Acquire IPMB address");
		addr = ipmi_acquire_ipmb_address(ipmi_main_intf_sdr);
		lprintf(LOG_INFO,  "Discovered IPMB address 0x%x", addr);
	}

	/*
	 * If we discovered the ipmb address and it is not the same as what we
	 * used for open, Set the discovered IPMB address as my address if the
	 * interface supports it.
	 */
	if (addr != 0 && addr != ipmi_main_intf_sdr->my_addr) {
		if (ipmi_main_intf_sdr->set_my_addr) {
			/*
			 * Some interfaces need special handling
			 * when changing local address
			 */
			(void)ipmi_main_intf_sdr->set_my_addr(ipmi_main_intf_sdr, addr);
		}

		/* set local address */
		ipmi_main_intf_sdr->my_addr = addr;
	}

	ipmi_main_intf_sdr->target_addr = ipmi_main_intf_sdr->my_addr;

	/* If bridging addresses are specified, handle them */
	if (transit_addr > 0 || target_addr > 0) {
		/* sanity check, transit makes no sense without a target */
		if ((transit_addr != 0 || transit_channel != 0) &&
			target_addr == 0) {
			lprintf(LOG_ERR,
				"Transit address/channel %#x/%#x ignored. "
				"Target address must be specified!",
				transit_addr, transit_channel);
			goto out_free;
		}
		ipmi_main_intf_sdr->target_addr = target_addr;
		ipmi_main_intf_sdr->target_channel = target_channel ;

		ipmi_main_intf_sdr->transit_addr    = transit_addr;
		ipmi_main_intf_sdr->transit_channel = transit_channel;


		/* must be admin level to do this over lan */
		ipmi_intf_session_set_privlvl(ipmi_main_intf_sdr, IPMI_SESSION_PRIV_ADMIN);
		/* Get the ipmb address of the targeted entity */
		ipmi_main_intf_sdr->target_ipmb_addr =
					ipmi_acquire_ipmb_address(ipmi_main_intf_sdr);
		lprintf(LOG_DEBUG, "Specified addressing     Target  %#x:%#x Transit %#x:%#x",
					   ipmi_main_intf_sdr->target_addr,
					   ipmi_main_intf_sdr->target_channel,
					   ipmi_main_intf_sdr->transit_addr,
					   ipmi_main_intf_sdr->transit_channel);
		if (ipmi_main_intf_sdr->target_ipmb_addr) {
			lprintf(LOG_INFO, "Discovered Target IPMB-0 address %#x",
					   ipmi_main_intf_sdr->target_ipmb_addr);
		}
	}

	/* set target LUN (for RAW command) */
	ipmi_main_intf_sdr->target_lun = target_lun ;

	lprintf(LOG_DEBUG, "Interface address: my_addr %#x "
			   "transit %#x:%#x target %#x:%#x "
			   "ipmb_target %#x\n",
			ipmi_main_intf_sdr->my_addr,
			ipmi_main_intf_sdr->transit_addr,
			ipmi_main_intf_sdr->transit_channel,
			ipmi_main_intf_sdr->target_addr,
			ipmi_main_intf_sdr->target_channel,
			ipmi_main_intf_sdr->target_ipmb_addr);

	/* parse local SDR cache if given */
	if (sdrcache != NULL) {
		ipmi_sdr_list_cache_fromfile(ipmi_main_intf_sdr, sdrcache);
	}
	/* Parse SEL OEM file if given */
	if (seloem != NULL) {
		ipmi_sel_oem_init(seloem);
	}

	/* Enable Big Buffer when requested */
	if ( my_long_packet_size != 0 ) {
		/* Enable Big Buffer when requested */
		if (!ipmi_oem_active(ipmi_main_intf_sdr, "kontron") ||
			ipmi_kontronoem_set_large_buffer(ipmi_main_intf_sdr,
					my_long_packet_size ) == 0) {
			printf("Setting large buffer to %i\n", my_long_packet_size);
			my_long_packet_set = 1;
			ipmi_intf_set_max_request_data_size_sdr(ipmi_main_intf_sdr,
					my_long_packet_size);
		}
	}

	ipmi_main_intf_sdr->cmdlist = cmdlist;	

	if (my_long_packet_set == 1) {
		if (ipmi_oem_active(ipmi_main_intf_sdr, "kontron")) {
			/* Restore defaults */
			ipmi_kontronoem_set_large_buffer( ipmi_main_intf_sdr, 0 );
		}
	}

	
	out_free:
	log_halt();

	if (intfname != NULL) {
		free(intfname);
		intfname = NULL;
	}
	if (hostname != NULL) {
		free(hostname);
		hostname = NULL;
	}
	if (username != NULL) {
		free(username);
		username = NULL;
	}
	if (password != NULL) {
		free(password);
		password = NULL;
	}
	if (oemtype != NULL) {
		free(oemtype);
		oemtype = NULL;
	}
	if (seloem != NULL) {
		free(seloem);
		seloem = NULL;
	}
	if (sdrcache != NULL) {
		free(sdrcache);
		sdrcache = NULL;
	}
	if (devfile) {
		free(devfile);
		devfile = NULL;
	}

	return rc;
}

void close_session(){
	/* clean repository caches */
	ipmi_cleanup(ipmi_main_intf_sdr);

	/* call interface close function if available */
	if (ipmi_main_intf_sdr->opened > 0 && ipmi_main_intf_sdr->close != NULL)
		ipmi_main_intf_sdr->close(ipmi_main_intf_sdr);
}


