#ifndef __SECURITY_H_820D0C9F_A563_489b_834A_23FBB6C892F7
#define __SECURITY_H_820D0C9F_A563_489b_834A_23FBB6C892F7

#include <windows.h>

enum
{
	WOEM_PROC_AUTHORIZATION_OK,
	WOEM_PROC_AUTHORIZATION_DENIED,
	WOEM_PROC_AUTHORIZATION_FAILURE,
};

/*!
	\brief verify if the current process is authorized to use WOEM

	\param errString A caller allocated string (with a minimum size of
	\ref PACKET_ERRSTR_SIZE bytes), that will return a meaningful error
	message in case the function returns WOEM_PROC_AUTHORIZATION_FAILURE.

	\return Possible return values are
	- \ref WOEM_PROC_AUTHORIZATION_FAILURE An error occurred verifying the 
	authoerization. A more detailed error is detailed in errString
	- \ref WOEM_PROC_AUTHORIZATION_OK	The current process is authorized
	- \ref WOEM_PROC_AUTHORIZATION_DENIED The current process is not authorized
*/
INT WoemGetCurrentProcessAuthorization(char *errString);

#endif //#ifndef __SECURITY_H_820D0C9F_A563_489b_834A_23FBB6C892F7
