#include <windows.h>
#include <wincrypt.h>

#include "Crypto.h"


VOID ShowMessage(LPCSTR lpText, LPCSTR lpCaption);
INT Error(LPCSTR lpText, LPCSTR lpCaption, DWORD errorCode);


INT getHash(LPCBYTE lpbData, DWORD dwDataLen, LPBYTE lpValue) {
	DWORD dwBufferSize = sizeof(DWORD);
	DWORD dwHashSize;
	HCRYPTPROV hCryptProv;
	HCRYPTHASH hHash;

	// initialize hash object
	if ( CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0) == FALSE ) 
		return Error("Crypto.initHashObject", "Error in CryptAcquireContext!", GetLastError());

	if ( CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash) == FALSE ) 
		return Error("Crypto.initHashObject", "Error in CryptCreateHash!", GetLastError());

	// add dump to hash object
	if ( CryptHashData(hHash, lpbData, dwDataLen, 0) == FALSE ) 
		return Error("Crypto.getHash", "Error in CryptCreateHash!", GetLastError());

	// get the size of the hash
	if ( CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&dwHashSize, &dwBufferSize, 0) == FALSE )
		return Error("Crypto.getHash", "Error in CryptGetHashParam!", GetLastError());

	if ( dwHashSize != CREDENTIAL_LEN )
		return Error("Crypto.getHash", "Error for different size of hash value!", FALSE);
	
	// get the hash of dump
	if ( CryptGetHashParam(hHash, HP_HASHVAL, lpValue, &dwHashSize, 0) == FALSE )
		return Error("Crypto.getHash", "Error in CryptGetHashParam!", GetLastError());

	// terminate hash object
	if ( hHash )
		CryptDestroyHash(hHash);
	if ( hCryptProv )
		CryptReleaseContext(hCryptProv, 0);

	return TRUE;
}
