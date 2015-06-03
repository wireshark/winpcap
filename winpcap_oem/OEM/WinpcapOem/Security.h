/*
 * Copyright 2015 Riverbed Technology, San Francisco (California).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Riverbed Technology nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

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
