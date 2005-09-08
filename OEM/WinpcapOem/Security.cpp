#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "Security.h"
#include "resource.h"
#include "Crypto.h"

#define BUF_LEN 256
#define DUMP_LEN 1024*10
#define MSG_BUF_LEN 255

BOOL isProcAuthorized = FALSE;

extern HINSTANCE DllHandle;

INT getFileCredential(LPCSTR lpFilename, LPBYTE credential);
INT isCredentialValid(LPBYTE Credential);
INT getFileDump(LPCSTR lpFilename, LPBYTE dump, INT maxDumpLen);


VOID ShowMessage(LPCSTR lpCaption, LPCSTR lpText) {
	CHAR buf[MSG_BUF_LEN+1];
	_snprintf(buf, MSG_BUF_LEN, "%s: %s", lpCaption, lpText);
	OutputDebugString(buf);
}

INT Error(LPCSTR lpCaption, LPCSTR lpText, DWORD errorCode) {
	CHAR buf[MSG_BUF_LEN+1];
	_snprintf(buf, MSG_BUF_LEN, "%s (errorcode: %d)", lpText, errorCode);
	ShowMessage(lpCaption, lpText);
	return errorCode;
}


// set the authorization flag for the current process
INT setProcAuthorization() {
	CHAR filename[BUF_LEN];
	HMODULE procHandle;
	BYTE credential[CREDENTIAL_LEN];

	ShowMessage("Security.setProcAuthorization", "--> START");
	
	// get the handle of the current process (NULL for the current process)
	procHandle = NULL;

	// get the name of the process image file
	if ( GetModuleFileName(procHandle, filename, BUF_LEN) == FALSE ) {
		return Error("Security.setProcAuthorization", "Error in GetModuleFileName!", GetLastError());
	}

	// get the credential of the process
	getFileCredential(filename, credential);

	// set the authorization flag
	isCredentialValid(credential);

	ShowMessage("Security.setProcAuthorization", "--> END");

	return TRUE;
}

// get file credential
INT getFileCredential(LPCSTR lpFilename, LPBYTE credential) {
	CHAR buf[MSG_BUF_LEN+1];
	BYTE dump[DUMP_LEN];

	_snprintf(buf, MSG_BUF_LEN, "Getting the dump of the file %s...", lpFilename);
	ShowMessage("Security.getFileCredential", buf);

	// get the dump of the file
	if ( getFileDump(lpFilename, dump, DUMP_LEN) == FALSE)
		return Error("Security.getFileCredential", "Error in getFileDump!", GetLastError());

	ShowMessage("Security.getFileCredential", "Getting the hash of the dump...");
	// get the hash of the dump
	if ( getHash(dump, DUMP_LEN, credential) == FALSE )
		return Error("Security.getFileCredential", "Error in getHash!", GetLastError());

	_snprintf(buf, MSG_BUF_LEN, "Got the hash %.08x... of the file %s!", *((DWORD*)credential), lpFilename);
	ShowMessage("Security.getFileCredential", buf);

	return TRUE;
}

// check the credential of the file
INT isCredentialValid(LPBYTE credential) {
	CHAR buf[MSG_BUF_LEN+1];
	LPBYTE lpResMem;
	DWORD dwResSize;
	DWORD dwResIndex=0;

	// get the stored credential of the file
	if ( WoemGetResource(DllHandle, HSH_AUTH_PROC, (void**)&lpResMem, &dwResSize) == FALSE )
		return Error("Security.isCredentialValid", "Error in WoemGetResource!", GetLastError());

	dwResSize=dwResSize/CREDENTIAL_LEN;

	_snprintf(buf, MSG_BUF_LEN, "Read %d credentials (%d bytes) from resources!", dwResSize, dwResSize*CREDENTIAL_LEN);
	ShowMessage("Security.isCredentialValid", buf);

	// compare the two credentials
	for (dwResIndex=0; dwResIndex < dwResSize; dwResIndex++) {
		_snprintf(buf, MSG_BUF_LEN, "Analyzing credential #%d (%.08x)...", dwResIndex+1, *((DWORD*)(lpResMem+dwResIndex*CREDENTIAL_LEN)));
		ShowMessage("Security.isCredentialValid", buf);
		if ( memcmp(credential, lpResMem+(dwResIndex*CREDENTIAL_LEN), CREDENTIAL_LEN) ==0 ) {
			isProcAuthorized=TRUE;
			ShowMessage("Security.isCredentialValid", "Found equal credential!");
			break;
		}
	}

	return TRUE;
}

// get the dump of the file
INT getFileDump(LPCSTR lpFilename, LPBYTE dump, INT maxDumpLen) {
	CHAR buf[MSG_BUF_LEN+1];
	FILE * fileHandle;
	INT byteRead;

	_snprintf(buf, MSG_BUF_LEN, "Opening the file %s...", lpFilename);
	ShowMessage("Security.getFileDump", buf);

	if ( (fileHandle = fopen(lpFilename, "rb")) == FALSE)
		return Error("Security.getFileDump", "Error in _open!", GetLastError());

	_snprintf(buf, MSG_BUF_LEN, "Reading the file %s...", lpFilename);
	ShowMessage("Security.getFileDump", buf);

	byteRead=fread(dump, 1, maxDumpLen, fileHandle);

	_snprintf(buf, MSG_BUF_LEN, "Read %d bytes from the file...", byteRead);
	ShowMessage("Security.getFileDump", buf);

	if ( byteRead < maxDumpLen )
		if ( feof(fileHandle) == FALSE )
			return Error("Security.getFileDump", "Error in ReadFile!", GetLastError());

	return TRUE;
}