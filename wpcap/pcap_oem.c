#include <pcap-int.h>

/*!
	\name OEM-specific Functions
*/
//\{ 

/*!
	\ingroup wpcapfunc 
	\brief This function starts WinPcap Professional.
	
	\param err_str pointer to a user-allocated buffer (of minimum size PCAP_ERRBUF_SIZE) 
		that will contain the error message in case of failure.
	\param flags Reserved. It should be set to 0.

	\return 0 on success,  -1 in case of failure.
*/
int pcap_start_oem(char* err_str, int flags)
{
	if(PacketStartOem(err_str, PCAP_ERRBUF_SIZE) == FALSE)
	{
		return -1;
	}
	return 0;
}


//\}
