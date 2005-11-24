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
