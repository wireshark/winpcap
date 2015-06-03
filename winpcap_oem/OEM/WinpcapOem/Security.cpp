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

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "Security.h"
#include "resource.h"
#include "Crypto.h"

#include "WinPcapOem.h"
#include "WoemDebug.h"

#define BUF_LEN 256
#define DUMP_LEN 1024*10

extern HINSTANCE g_DllHandle;

static BOOL GetFileCredential(LPCSTR lpFilename, LPBYTE credential, DWORD credentialLength, char* errString);
static INT IsCredentialValid(LPBYTE credential, DWORD credentialLength);
static BOOL GetFileDump(LPCSTR lpFilename, LPBYTE dump, INT *pMaxDumpLen);

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
INT WoemGetCurrentProcessAuthorization(char *errString) 
{
	CHAR filename[MAX_PATH];
	HMODULE procHandle;
	BYTE credential[CREDENTIAL_LEN];
	INT result;

	TRACE_ENTER("WoemGetCurrentProcessAuthorization");
	
	//handle of the process, NULL means "this process"
	procHandle = NULL;

	// get the name of the process image file
	if ( GetModuleFileName(procHandle, filename, MAX_PATH) == FALSE ) 
	{
		_snprintf(errString, PACKET_ERRSTR_SIZE - 1,"WoemGetCurrentProcessAuthorization, Error retrieving the module filename with GetModuleFileName [%d].", GetLastError());
		errString[PACKET_ERRSTR_SIZE - 1] = 0;

		TRACE_MESSAGE(errString);

		TRACE_EXIT("WoemGetCurrentProcessAuthorization");
		return WOEM_PROC_AUTHORIZATION_FAILURE;
	}

	// get the credential of the process
	if (GetFileCredential(filename, credential, sizeof(credential), errString) == FALSE)
	{
		TRACE_EXIT("WoemGetCurrentProcessAuthorization");
		return WOEM_PROC_AUTHORIZATION_FAILURE;
	}

	// set the authorization flag
	result = IsCredentialValid(credential, sizeof(credential));

	if (result == WOEM_PROC_AUTHORIZATION_FAILURE)
	{
		_snprintf(errString, PACKET_ERRSTR_SIZE - 1,"WoemGetCurrentProcessAuthorization, Error verifying the credentials of the current process");
		errString[PACKET_ERRSTR_SIZE - 1] = 0;
	}

	TRACE_EXIT("WoemGetCurrentProcessAuthorization");

	return result;
}

// get file credential
BOOL GetFileCredential(LPCSTR lpFilename, LPBYTE credential, DWORD credentialLength, char* errString) 
{
	BYTE dump[DUMP_LEN];
	INT dumpSize;

	TRACE_ENTER("GetFileCredential");
	TRACE_MESSAGE("Getting the dump of the file %s...", lpFilename);

	// get the dump of the file
	dumpSize = sizeof(dump);
	
	if (GetFileDump(lpFilename, dump, &dumpSize) == FALSE)
	{
		_snprintf(errString, PACKET_ERRSTR_SIZE - 1,"GetFileCredential, error analyzing the process file.");
		errString[PACKET_ERRSTR_SIZE - 1] = 0;
	
		TRACE_MESSAGE("GetFileCredential, error getting the file dump.");
		TRACE_EXIT("GetFileCredential");
		return FALSE;
	}

	TRACE_MESSAGE("GetFileCredential, Getting the hash of the dump...");

	// get the hash of the dump
	if ( GetHash(dump, dumpSize, credential, credentialLength) == FALSE )
	{
		_snprintf(errString, PACKET_ERRSTR_SIZE - 1,"GetFileCredential, error analyzing the process file.");
		errString[PACKET_ERRSTR_SIZE - 1] = 0;
	
		TRACE_MESSAGE("GetFileCredential, error in getHash!");
		TRACE_EXIT("GetFileCredential");
		return FALSE;
	}
	
	TRACE_MESSAGE("GetFileCredential, Got the hash %.08x... of the file %s!", *((DWORD*)credential), lpFilename);

	TRACE_EXIT("GetFileCredential");
	return TRUE;
}

/*!
	\brief Verify that the given credential is valid, i.e. it's in the list
	of valid ones in the resource file.

	\param credential Caller allocated buffer containing the credential to be verified
	\param credentialLength Length of the credential to be verified

	\return Possible return values are
	- \ref WOEM_PROC_AUTHORIZATION_FAILURE An error occurred verifying the 
	authoerization.
	- \ref WOEM_PROC_AUTHORIZATION_OK	The credential is authorized
	- \ref WOEM_PROC_AUTHORIZATION_DENIED The credential is not authorized
*/
INT IsCredentialValid(LPBYTE credential, DWORD credentialLength) 
{
	LPBYTE lpResMem;
	DWORD dwResSize;
	
	DWORD numCredentials;
	DWORD credentialIndex;

	TRACE_ENTER("IsCredentialValid");

	if (credentialLength != CREDENTIAL_LEN)
	{
		TRACE_MESSAGE("Security.IsCredentialValid, the credential does not have the right length!");
		TRACE_EXIT("IsCredentialValid");
		
		return WOEM_PROC_AUTHORIZATION_FAILURE;
	}

	// get the stored credential of the file
	if (WoemGetResource(g_DllHandle, HSH_AUTH_PROC, (void**)&lpResMem, &dwResSize) == FALSE )
	{
		TRACE_MESSAGE("Security.IsCredentialValid, Error in WoemGetResource [%.08x]!\n", GetLastError());
		TRACE_EXIT("IsCredentialValid");
		
		return WOEM_PROC_AUTHORIZATION_FAILURE;
	}

	numCredentials = dwResSize/CREDENTIAL_LEN;

	TRACE_MESSAGE("Read %d credentials (%d bytes) from resources!", numCredentials, dwResSize);

	// compare the two credentials
	for (credentialIndex = 0; credentialIndex < numCredentials; credentialIndex++) 
	{
		TRACE_MESSAGE("IsCredentialValie, Analyzing credential #%d (%.08x)...", credentialIndex, *((DWORD*)(lpResMem+credentialIndex*CREDENTIAL_LEN)));
		
		if ( memcmp(credential, lpResMem+(credentialIndex * CREDENTIAL_LEN), CREDENTIAL_LEN) == 0 ) 
		{
			TRACE_MESSAGE("Security.isCredentialValid, Found equal credential!");
			
			TRACE_EXIT("IsCredentialValid");
			return WOEM_PROC_AUTHORIZATION_OK;
		}
	}

	TRACE_EXIT("IsCredentialValid");
	return WOEM_PROC_AUTHORIZATION_DENIED;
}

// get the dump of the file
BOOL GetFileDump(LPCSTR lpFilename, LPBYTE dump, INT *pMaxDumpLen) 
{
	FILE * fileHandle;
	size_t byteRead;
	BOOL fileError;

	TRACE_ENTER("GetFileDump");

	if (*pMaxDumpLen <= 0)
	{
		TRACE_MESSAGE("GetFileDump, MacDumpLen <= 0");
		TRACE_EXIT("GetFileDump");

		return FALSE;
	}
	
	TRACE_MESSAGE("Opening the file %s...", lpFilename);

	if ( (fileHandle = fopen(lpFilename, "rb")) == FALSE)
	{
		TRACE_MESSAGE("GetFileDump, Error in fopen [%.08x]!", GetLastError());
		TRACE_EXIT("GetFileDump");

		return FALSE;
	}

	byteRead = 0;
	fileError = FALSE;

	TRACE_MESSAGE("Reading the file %s...", lpFilename);

	do
	{
		size_t localBytesRead = fread(dump + byteRead, 1, *pMaxDumpLen - byteRead, fileHandle);
		byteRead += localBytesRead;

		if (localBytesRead == 0)
		{
			if (feof(fileHandle))
				fileError = FALSE;
			else
				fileError = TRUE;

			break;
		}
		
	}
	while (byteRead < *pMaxDumpLen);

	fclose(fileHandle);

	TRACE_MESSAGE("GetFileDump, Read %d bytes from the file...", byteRead);

	if (fileError)
	{
		TRACE_MESSAGE("Error reading the bytes from the file.");

		TRACE_EXIT("GetFileDump");

		return FALSE;
	}

	*pMaxDumpLen = (INT)byteRead;

	TRACE_EXIT("GetFileDump");

	return TRUE;
}
