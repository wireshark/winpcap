#include <pcap-int.h>

/*!
	\name Functions specific to WinPcap Professional
*/
//\{ 

/*!
	\ingroup wpcapfunc
	\brief Flag used in conjunction with \ref pcap_start_oem.
	When this flag is used, WinPcap Professional will be started without support
	for dialup and VPN adapters (i.e. without loading the Microsoft NetMon component).
*/
#define PCAP_START_OEM_NO_NETMON	0x00000001

/*!
	\ingroup wpcapfunc 
	\brief This function starts WinPcap Professional.
	
	\param err_str pointer to a user-allocated buffer (of minimum size PCAP_ERRBUF_SIZE) 
		that will contain the error message in case of failure.
	\param flags A bitwise combination of the following flags:
		- \ref PCAP_START_OEM_NO_NETMON
	\return 0 on success,  -1 in case of failure.
*/
int pcap_start_oem(char* err_str, int flags)
{
	if(PacketStartOemEx(err_str, PCAP_ERRBUF_SIZE, (ULONG)flags) == FALSE)
	{
		return -1;
	}
	return 0;
}


//\}
