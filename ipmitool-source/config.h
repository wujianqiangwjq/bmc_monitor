/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define to 1 to enable all command line options. */
#define ENABLE_ALL_OPTIONS 1

/* Define to 1 for extra file security. */
/* #undef ENABLE_FILE_SECURITY */

/* Define to 1 to enable OpenIPMI interface dual bridge support */
/* #undef ENABLE_INTF_OPEN_DUAL_BRIDGE */

/* Define to 1 if you have the `alarm' function. */
#define HAVE_ALARM 1

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define to 1 if you have the <byteswap.h> header file. */
#define HAVE_BYTESWAP_H 1

/* Define to 1 if libcrypto supports MD2. */
#define HAVE_CRYPTO_MD2 1

/* Define to 1 if libcrypto supports MD5. */
#define HAVE_CRYPTO_MD5 1

/* Define to 1 if libcrypto supports SHA256. */
#define HAVE_CRYPTO_SHA256 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <sys/ipmi.h> header file. */
/* #undef HAVE_FREEBSD_IPMI_H */

/* Define to 1 if you have the `getaddrinfo' function. */
#define HAVE_GETADDRINFO 1

/* Define to 1 if you have the `gethostbyname' function. */
#define HAVE_GETHOSTBYNAME 1

/* Define to 1 if you have the `getifaddrs' function. */
#define HAVE_GETIFADDRS 1

/* Define to 1 if you have the `getpassphrase' function. */
/* #undef HAVE_GETPASSPHRASE */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <linux/compiler.h> header file. */
/* #undef HAVE_LINUX_COMPILER_H */

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the <linux/ipmi.h> header file. */
#define HAVE_OPENIPMI_H 1

/* Define to 1 if you have the <paths.h> header file. */
#define HAVE_PATHS_H 1

/* Define to 1 if you need to use #pragma pack instead of __attribute__
   ((packed)) */
#define HAVE_PRAGMA_PACK 1

/* Define to 1 if readline present. */
/* #undef HAVE_READLINE */

/* Define to 1 if you have the `select' function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the `socket' function. */
#define HAVE_SOCKET 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/byteorder.h> header file. */
/* #undef HAVE_SYS_BYTEORDER_H */

/* Define to 1 if you have the <sys/ioccom.h> header file. */
/* #undef HAVE_SYS_IOCCOM_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have <sys/termios.h>. */
/* #undef HAVE_SYS_TERMIOS_H */

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have <termios.h>. */
#define HAVE_TERMIOS_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 to enable Solaris 10 BMC interface. */
/* #undef IPMI_INTF_BMC */

/* Define to 1 to enable Dummy interface. */
/* #undef IPMI_INTF_DUMMY */

/* Define to 1 to enable FreeIPMI interface. */
/* #undef IPMI_INTF_FREE */

/* Define to 1 for FreeIPMI 0.3.0. */
/* #undef IPMI_INTF_FREE_0_3_0 */

/* Define to 1 for FreeIPMI 0.4.0. */
/* #undef IPMI_INTF_FREE_0_4_0 */

/* Define to 1 for FreeIPMI 0.5.0. */
/* #undef IPMI_INTF_FREE_0_5_0 */

/* Define to 1 for FreeIPMI 0.6.0. */
/* #undef IPMI_INTF_FREE_0_6_0 */

/* Define to 1 to enable FreeIPMI Bridging Support. */
/* #undef IPMI_INTF_FREE_BRIDGING */

/* Define to 1 to enable Intel IMB interface. */
#define IPMI_INTF_IMB 1

/* Define to 1 to enable LAN IPMIv1.5 interface. */
#define IPMI_INTF_LAN 1

/* Define to 1 to enable LAN+ IPMIv2 interface. */
#define IPMI_INTF_LANPLUS 1

/* Define to 1 to enable Solaris 9 LIPMI interface. */
/* #undef IPMI_INTF_LIPMI */

/* Define to 1 to enable Linux OpenIPMI interface. */
#define IPMI_INTF_OPEN 1

/* Define to 1 to enable serial interface. */
#define IPMI_INTF_SERIAL 1

/* Define to 1 to enable USB interface. */
/* #undef IPMI_INTF_USB */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "ipmitool"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "ipmitool"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "ipmitool 1.8.18-csv"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "ipmitool"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.8.18-csv"

/* Define to the type of arg 1 for `select'. */
#define SELECT_TYPE_ARG1 int

/* Define to the type of args 2, 3 and 4 for `select'. */
#define SELECT_TYPE_ARG234 (fd_set *)

/* Define to the type of arg 5 for `select'. */
#define SELECT_TYPE_ARG5 (struct timeval *)

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "1.8.18-csv"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif
