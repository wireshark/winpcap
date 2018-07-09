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
#include <wincrypt.h>

#include "Crypto.h"
#include "WoemDebug.h"

BOOL GetHash(LPCBYTE lpbData, DWORD dwDataLen, LPBYTE lpValue, DWORD dwValueLength) {
	DWORD dwBufferSize = sizeof(DWORD);
	DWORD dwHashSize;
	HCRYPTPROV hCryptProv = NULL;
	HCRYPTHASH hHash = NULL;

	TRACE_ENTER("GetHash");

	// initialize hash object
	if ( CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) == FALSE ) 
	{
		TRACE_MESSAGE("GetHash, Error in CryptAcquireContext [%.08x]", GetLastError());

		TRACE_EXIT("GetHash");
		return FALSE;
	}



	if ( CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash) == FALSE ) 
	{
		TRACE_MESSAGE("GetHash, Error in CryptCreateHash [%.08x]", GetLastError());

		CryptReleaseContext(hCryptProv, 0);
		
		TRACE_EXIT("GetHash");
		return FALSE;
	}

	// add dump to hash object
	if ( CryptHashData(hHash, lpbData, dwDataLen, 0) == FALSE ) 
	{
		TRACE_MESSAGE("GetHash, Error in CryptHashData [%.08x]", GetLastError());

		CryptDestroyHash(hHash);
		CryptReleaseContext(hCryptProv, 0);
		
		TRACE_EXIT("GetHash");
		return FALSE;
	}

	// get the size of the hash
	if ( CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&dwHashSize, &dwBufferSize, 0) == FALSE )
	{
		TRACE_MESSAGE("GetHash, Error in CryptGetHashParam [%.08x]", GetLastError());

		CryptDestroyHash(hHash);
		CryptReleaseContext(hCryptProv, 0);
		
		TRACE_EXIT("GetHash");
		return FALSE;
	}

	if ( dwHashSize != CREDENTIAL_LEN || dwValueLength != CREDENTIAL_LEN )
	{
		TRACE_MESSAGE("GetHash, Error for different size of hash value (CryptAPI says %d, we have %d)", dwHashSize, CREDENTIAL_LEN );

		CryptDestroyHash(hHash);
		CryptReleaseContext(hCryptProv, 0);
		
		TRACE_EXIT("GetHash");
		return FALSE;
	}
	
	// get the hash of dump
	if ( CryptGetHashParam(hHash, HP_HASHVAL, lpValue, &dwHashSize, 0) == FALSE )
	{
		TRACE_MESSAGE("GetHash, Error in CryptGetHashParam [%.08x]", GetLastError());

		CryptDestroyHash(hHash);
		CryptReleaseContext(hCryptProv, 0);
		
		TRACE_EXIT("GetHash");
		return FALSE;
	}

	// terminate hash object
	CryptDestroyHash(hHash);
	CryptReleaseContext(hCryptProv, 0);

	TRACE_EXIT("GetHash");
	return TRUE;
}
