Name:         ipmitool
Summary:      ipmitool - Utility for IPMI control
Version:      1.8.18-csv
Release:      1%{?_distro:.%{_distro}}
License:      BSD
Group:        Utilities
Packager:     Jim Mankovich <jmank@hp.com>
Source:       %{name}-%{version}.tar.gz
Buildroot:    /var/tmp/ipmitool-root

%description
This package contains a utility for interfacing with devices that support
the Intelligent Platform Management Interface specification.  IPMI is
an open standard for machine health, inventory, and remote power control.

This utility can communicate with IPMI-enabled devices through either a
kernel driver such as OpenIPMI or over the RMCP LAN protocol defined in
the IPMI specification.  IPMIv2 adds support for encrypted LAN
communications and remote Serial-over-LAN functionality.

It provides commands for reading the Sensor Data Repository (SDR) and
displaying sensor values, displaying the contents of the System Event
Log (SEL), printing Field Replaceable Unit (FRU) information, reading and
setting LAN configuration, and chassis power control.

%prep
if [ "$RPM_BUILD_ROOT" ] && [ "$RPM_BUILD_ROOT" != "/" ]; then
    rm -rf $RPM_BUILD_ROOT
fi

%setup

%build
./configure --with-kerneldir \
	--with-rpm-distro= \
	--prefix=%{_prefix} \
	--bindir=%{_bindir} \
	--sbindir=%{_sbindir} \
	--datadir=%{_datadir} \
	--includedir=%{_includedir} \
	--libdir=%{_libdir} \
	--mandir=%{_mandir} \
	--sysconfdir=%{_sysconfdir}
make

%install
make DESTDIR=$RPM_BUILD_ROOT install-strip

%clean
if [ "$RPM_BUILD_ROOT" ] && [ "$RPM_BUILD_ROOT" != "/" ]; then
    rm -rf $RPM_BUILD_ROOT
fi

%files
%defattr(755,root,root)
%attr(755,root,root) %{_bindir}/*
%attr(755,root,root) %{_sbindir}/*
%{_datadir}/ipmitool/*
%{_mandir}/man*/*
%doc %{_datadir}/doc/ipmitool


%changelog
* Wed Feb 25 2009 <pere@hungry.com>  1.8.11-1
- Fix new GCC compilation issues in regards to Packing
- Fix Tracker bug #1642710 - ipmi_kcs_drv being loaded/unloaded
  for 2.4 kernel instead of ipmi_si_drv driver module
- New -y option added to allow specification of kg keys with
  non-printable characters
- New -K option added to allow kgkey settings via environmental
  variable IPMI_KGKEY
- Generic device support added for EEPROM with SDR Type 10h (gendev)
- Fix to lan-bridging for a double-bridging crash and to fix
  an issue with bridging multiple concurrent requests and
  erroneous handling of raw Send Message
- Lanplus fix for commands like 'sensor list' without the -t option
  causing wrong double bridged requests of a sensor is located
  on another satellite controller
- Fix lan and lanplus request list entry removal bugs
- Fix non-working issue when trying to send a bridge message with
  Cipher 3
- Change bridge message handling to reuse command ipmi_lan_poll_recv
- Added PICMG 2.0 and 2.3 support
- Fix PICMG (ATCA) extension verification and reversal of BCD encoded
  values for "major" and "minor" fields
- Add IANA support for Pigeon Point
- Add OEM SW/FW Record identification
- Fix to include I2C and LUN addresses so sensors are correctly managed
- Patch ID 1990560 to get readings from non-linear analog sensors
- Add support for SOL payload status command
- SOL set parameter range checking added
- Fixed SOL activate options usage 
- Fixed crashes when parsing 'sol payload' and 'tsol' cmds (#216967)
- Added retries to SOL keepalive
- Fixed wrong mask values for Front Panel disable/enable status
- Add support to access fru internal use area
- Add support for new PICMG 3.0 R3.0 (March 24, 2008) to allow
  blocks of data within the FRU storage area to be write protected.
- Fix node reporting in GUID; Tracker bug #2339675
- Fix watchdog use/action print strings
- Fix endian bug in SDR add from file; Tracker bug #2075258
- Fix crash when dumping SDRs in a file and there's an error
  getting an SDR; improve algorithm for optimal packet size
- Fix occasional SDR dump segfault; #1793076
- Allow ipmitool sel delete to accept hex list entry numbers
- Fix SEL total space reporting.
- Fix for garbage sensor threshold values reported when none 
  returned.  Tracker Bug #863748
- ipmievd change to Monitor %used in SEL buffer and log warnings when
  the buffer is 80% and 100% full

* Fri Aug 08 2008 <pere@hungry.com>  1.8.10-1
 - Added support for BULL IANA number.
 - Fixed contrib build so the oem_ibm_sel_map file gets included in rpm 
   builds again.
 - Added support for Debian packages to be built from CVS
 - Fix for sdr and sel timestamp reporting issues
 - Fix for discrete sensor state print routines to address state bits 8-14
 - Change ipmi_chassis_status() to non-static so it can be used externally
 - Added retries to SOL keepalive
 - Fix to stop sensor list command from reporting a failure due to missing 
   sensor
 - Fix bug in sdr free space reporting
 - Add support for IANA number to vendor name conversion for many vendors
 - Fix segfault bug in lan set command
 - Fix bug in population of raw i2c wdata buffer
 - Fix bug in ipmb sensor reading
 - Fix misspellings, typos, incorrect strncmp lengths, white space
 - Update/fix printed help and usages for many commands
 - Add and update support for all commands in ipmitool man page
 - Fix for lanplus session re-open when the target becomes unavailable following
   a fw upgrade activation
 - Add support for watchdog timer shutoff, reset, and get info
 - Add support for more ibm systems in oem_ibm_sel_map
 - Add more JEDEC support info for DIMMs; decrease request size for DIMM FRU
   info to 16 bytes at a time to allow more DIMM FRUs to respond.
 - Fix to change hpmfwupg to version 1.02; fix to reduce hpmfwupg buffer 
   length more aggressively when no response from iol
 - Fix HPM firmware activation via IOL; fake a timeout after IOL session 
   re-open to force get upgrade status retry; Added retries on 0xD3 
   completion code
 - Add support for freeipmi 0.6.0; adjust autoconf for changes
 - Fix for oemval2str size
 - Add support for product name resolution in mc info
 - Fix FRU display format
 - Added PICMG ekeying analyzer module support (ekanalyzer); display point
   to point physical connectivity and power supply information between 
   carriers and AMC modules; display matched results of ekeying match 
   between an on-carrier device and AMC module or between 2 AMC modules
 - Fix AMC GUID display support
 - Improved amcportstate operations
 - Added resolution for new sensor types
 - Fix segfault in SOL
 - Fix bug that caused infinite loop on BMCs with empty SDRs
 - Fix to move out Kontron OEM sensor resolution for other OEMs which could 
   lead to bad event descriptions
 - Add new FRU edit mode thereby allowing serial numbers, etc. to be changed;
    improvements to OEM edit mode
 - Added SPD support for parms: channel number, max read size
 - Add SDR support for adding SDR records from a dumped file, clearing SDR, 
   adding partial SDR records
 - Add updates and fixes to hpmfwupg: upload block size to 32 bytes for KCS,
   handle long response option, implement rollback override, garbage output fix
 - Add double bridge lan support , fix bridging issue
 - Add HPM support to pre-check which components need to be skipped
 - Fix autodetection of maximum packet size when using IPMB
 - Add new Kontron OEM command to set the BIOS boot option sequence
 - Add support for dual-bridge/ dual send message
 - Add auto-detect for local IPMB address using PICMG 2.X extension
 - Add support for HPM.1 1.0 specification compliance
 - Fix for improper lan/lanplus addressing
 - Added transit_channel and transit_addr to ipmi_intf struct
 - Fix bad password assertion bug due to rakp2 HMAC not being checked properly
 - Added ability to interpret PPS shelf manager clia sel dump
 - Corrected PICMG M7 state event definition macros
 - Added FRU parsing enhancements
 - Added "isol info", "isol set" and "isol activate" commands to support 
   Intel IPMI v1.5 SOL functionality. Removed "isol setup" command.
 - Fix bug in ipmi_lan_recv_packet() in lan and lanplus interfaces.
 - Fix bug in "chassis poh" command.
 - Fix HPM.1 upgrade to apply to only given component when instructed to do so
 - Added configure auto-detection if dual bridge extension is supported 
   by OpenIPMI

* Tue Mar  6 2007 <pere@hungry.com>  1.8.9-1
 - Added initial AMC ekey query operation support
 - Improvements to ekeying support (PICMG 3.x only)
 - Added initial interactive edition support for multirec; added IANA
   verification before interpreting PICMG records.
 - Added edit support for AMC activation "Maximum Internal Current"
 - Fix bug generating garbage on the screen when handling GetDeviceId
   and sol traffic occurs
 - Added ability to map OEM sensor types to OEM description string using 
   IANA number; moved IANA number table
 - Fix lan set access command to use value already saved within parameters 
   for PEF and authentication
 - Fix bug in cmd ipmitool lan stats get 1
 - Add support to allow ipmitool/ipmievd to target specific device nodes 
   on multi-BMC systems
 - Add support for name+privilege lookup for lanplus sessions
 - Fix time_t conversion bug for 64-bit OS
 - Added prefix of hostname on sel ipmievd sessions
 - Fixed FWUM Get Info
 - Fix ipmievd fd closing bug
 - Add set-in-progress flag support to chassis bootdev
 - Added new chassis bootdev options
 - Add sol payload enable/disable comman
 - Fix SOL set errors when commit-write not supported
 - Fix reset of session timeout for lanplus interface
 - Fixed lan interface accessibility timeout handling
 - Fix bug with Function Get Channel Cipher Suites command when more 
   than 1 page used.
 - Fix missing firmware firewall top-level command
 - Fix bug in SOL keepalive functionality
 - Fix SOLv2 NACK and retry handling for Intel ESB2 BMC
 - Added ipmi_sel_get_oem_sensor* APIs
 - Added HPM.1 support 
 - Fix segfault when incorrect oem option supplied
 - Fix bus problem with spd command
 - Fix segfault in SOL when remote BMC does not return packet
 - Adjust packet length for AMC.0 retricting IPMB packets to 32 bytes
 - Added lan packet size reduction mechanism
 - Fix bug with sendMessage of bad length with different target
 - Fix for big endian (PPC) architecture
 - NetBSD fixes
 - Fix segfault and channel problem with user priv command
 - Add support for bus/chan on i2c raw command
 - Add freeipmi interface support
 - Add remote spd printing
 - Add better detection of linux/compiler.h to config
 - Makefile changes to fix makedistcheck, etc. 

* Tue May 02 2006 <duncan@iceblink.org>  1.8.8-1
 - Fix segfaults in sensor data repository list
 - Fix ipmievd to open interface before daemonizing
 - Fix IPMIv1.5 authtype NONE to ignore supplied password
 - Fix cipher suite display bug in lan print
 - Fix typo in IPMIv2 SOL output when sending break
 - Fix improper LUN handling with Tyan SOL
 - Add LUN support to OpenIPMI interface
 - Add support for Kontron OEM commands
 - Update to Kontron Firmware Update command

* Sun Mar 19 2006 <duncan@iceblink.org>  1.8.7-1
 - Add Sun OEM command for blades
 - Increase argument size for raw commands in shell/exec
 - Fix handling of LUNs for LAN interfaces
 - Add IPMIv2 SOL loopback test
 - Add support for IBM OEM SEL messages
 - Disable file paranoia checks on read files by default
 - Support IPMIv2 SOL on older Intel boxes
 - Display message and exit if keepalive fails during SOL
 - Add support for setting VLAN id and priority
 - Add support for FreeBSD OpenIPMI-compatible driver
 - Add support for IPMIv2 Firmware Firewall
 - Fix gcc4 compile warnings
 - Make ipmievd generate pidfile
 - Add initscripts for ipmievd

* Mon Jan 17 2006 <duncan@iceblink.org>  1.8.6-1
 - Fix memory corruption when sending encrypted SOL traffic
 - Add keepalive timer to IPMIv2 SOL sessions

* Sat Jan 14 2006 <duncan@iceblink.org>  1.8.5-1
 - Raise privilege level after creating IPMIv2 session
 - Add support for settable SOL escape character with -e option
 - Add support for Kg BMC key for IPMIv2 authentication with -k option
 - Add support for Tyan IPMIv1.5 SOL with tsol command
 - Add support for PICMG devices
 - Add support for OEM SEL event parsing
 - Add support for command bridging over lan and lanplus interfaces
 - New 'chassis selftest' command
 - Many bufxies and patches from contributors

* Wed May 18 2005 <duncan@iceblink.org>  1.8.2-1
 - Fix FRU reading for large (>255 bytes) areas.
 - Overhaul to ipmievd to support SEL polling in addition to OpenIPMI.
 - Fix LAN parameter segfault when no Ciphers supported by BMC.
 - Fix IPMIv2 support on Intel v2 BMCs (use -o intelplus).
 - Separate option parsing code from main ipmitool source file.
 - Add raw I2C support with IPMI Master Read-Write command.
 - Add support for new 'sdr elist' extended output format.
 - Add support for listing sensors by type with 'sdr type' command.
 - Add support for new 'sel elist' extended output format that
   cross-references events with sensors.
 - Add support for sending dynamically generated platform events
   based on existing sensor information.
 - New '-S' argument to read local SDR cache created with 'sdr dump'.
 - Updated manpage for ipmitool and ipmievd.

* Wed Apr 06 2005 <duncan@iceblink.org>  1.8.1-1
 - Install ipmievd into /usr/sbin

* Wed Mar 16 2005 <duncan@iceblink.org>  1.8.0-1
 - Fix IPMIv2.0 issues
 - Fix chassis boot parameter support
 - Add support for linear sensors
 - Update bmc plugin to work with new Solaris bmc driver (new ioctl
   for interface detection and new STREAMS message-based interface)

* Tue Jan 18 2005 <duncan@iceblink.org>  1.7.0-1
 - Propogate errors correctly so exit status will be useful
 - More consistent display of errors including completion code text
 - Errors and debug is send to stderr now
 - New "sel get" command that will print details about SEL entry
   and corresponding SDR records as well as FRUs via entity association
 - Improved event generator, now supports reading events from text file
 - New "-o oemtype" option for specifying OEM boards
   exsting types are "supermicro" and "intelwv2"
 - New PEF subsystem from Tim Murphy at Dell
 - New "bmc" plugin for Solaris 10 x86
 - Many bugfixes and contributed patches
 - Support for Supermicro BMC OEM authentication method
 - Fix minor problem with LAN parameter setting

* Wed Aug 18 2004 <duncan@iceblink.org>  1.6.0-1
 - Add a README
 - Add support for IPMIv2 and Serial-over-LAN from Newisys
 - Add Solaris x86 lipmi interface
 - Add support for building Solaris packages
 - Add support for building RPMs as non-root user
 - Fix segfault when doing "sel list" (from Matthew Braithwaite)
 - Fix "chassis identify" on some BMCs (from ebrower@sourceforge)
 - Add "bmc info" and related output (from ebrower@sourceforge)
 - new "shell" and "exec" commands
 - lots of other contributed patches

* Sat May 27 2004 <duncan@iceblink.org>  1.5.9-1
 - Add ability to get a particular sensor by name
 - Add ability to set a particular sensor threshold
 - Add support for displaying V2 channel authentication levels
 - Add README for rrdtool scripts in contrib directory
 - Improve lan interface retry handling
 - Support prompting for password or reading from environment
 - Move chaninfo command into channel subcommand
 - Fix reservation ID handling when two sessions open to BMC
 - Fix reading of large FRU data
 - Add configure option for changing binary to ipmiadm for Solaris
 - Fix compile problem on Solaris 8

* Tue Jan 27 2004 <duncan@iceblink.org>  1.5.8-1
 - Enable static compilation of interfaces
 - Fix types to be 64-bit safe
 - Fix compilation problems on Solaris
 - Fix multiple big-endian problems for Solaris/SPARC
 - Fix channel access to save settings to NVRAM
 - Set channel privilege limit to ADMIN during "access on"
 - Enable gratuitous ARP in bmcautoconf.sh
 - Add support for Linux kernel panic messages in SEL output
 - Add support for type 3 SDR records

* Mon Jan  5 2004 <duncan@iceblink.org>  1.5.7-1
 - add IPMIv1.5 eratta fixes
 - additions to FRU printing and FRU multirecords
 - better handling of SDR printing
 - contrib scripts for creating rrdtool graphs

* Thu Dec  4 2003 <duncan@iceblink.org>  1.5.6-1
 - Fix SEL event decoding for generic events
 - Handle empty SEL gracefully when doing "sel list"
 - Fix sdr handling of sensors that do not return a reading
 - Fix for CSV display of sensor readings/units from Fredrik �hrn

* Tue Nov 25 2003 <duncan@iceblink.org>  1.5.5-1
 - Add -U option for setting LAN username
 - Fix -v usage for plugin interfaces

* Fri Nov 14 2003 <duncan@iceblink.org>  1.5.4-1
 - pull interface plugin api into library
 - fix ipmievd

* Fri Oct 31 2003 <duncan@iceblink.org>  1.5.3-1
 - add -g optin for pedantic ipmi-over-lan communication

* Fri Oct 24 2003 <duncan@iceblink.org>  1.5.2-1
 - add gratuitous arp interval setting

* Wed Oct  8 2003 <duncan@iceblink.org>  1.5.1-1
 - better SEL support
 - fix display bug in SDR list

* Fri Sep  5 2003 <duncan@iceblink.org>  1.5.0-1
 - use automake/autoconf/libtool
 - dynamic loading interface plugins

* Wed May 28 2003 <duncan@iceblink.org>  1.4.0-1
 - make UDP packet handling more robust
 - fix imb driver support

* Thu May 22 2003 <duncan@iceblink.org>  1.3-1
 - update manpage
 - rework of low-level network handling
 - add basic imb driver support

* Wed Apr  2 2003 <duncan@iceblink.org>  1.2-1
 - change command line option parsing
 - support for more chassis commands

* Tue Apr  1 2003 <duncan@iceblink.org>  1.1-1
 - minor fixes.

* Sun Mar 30 2003 <duncan@iceblink.org>  1.0-1
 - Initial release.

