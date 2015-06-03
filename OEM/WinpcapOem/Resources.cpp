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
#include <strsafe.h>
#include "WinpcapOem.h"
#include "WoemDebug.h"

////////////////////////////////////////////////////////////////////
// This function locates one of the binary resources embedded in the
// dll and saves it to disk
////////////////////////////////////////////////////////////////////
BOOL WoemSaveResourceToDisk(HINSTANCE hInst, int ResID, char* FileName, BOOL bDisableFsRedirector)
{
	DWORD dwLastError = ERROR_SUCCESS;
	HANDLE hFile = INVALID_HANDLE_VALUE;

#ifdef _X86_	
	PVOID OldFsRedirectorValue = NULL;
	HMODULE hKernel32Dll = NULL;
	Wow64DisableWow64FsRedirectionHandler DisableFsRedirector = NULL;
	Wow64RevertWow64FsRedirectionHandler RevertFsRedirector = NULL;
#endif
	CHAR ResName[100];

#ifdef STATIC_LIB
	hInst = NULL;
#endif

	TRACE_ENTER("WoemSaveResourceToDisk");

	StringCchPrintfA(ResName, sizeof(ResName), "#%d", ResID);

	TRACE_MESSAGE("Decompressing to file %s", FileName);

	do
	{
#ifdef _X86_
		if (bDisableFsRedirector)
		{
			//
			// load the FS redirector function dynamically
			//
			hKernel32Dll = LoadLibrary("kernel32.dll");

			if (hKernel32Dll == NULL)
			{
				dwLastError = GetLastError();
				TRACE_MESSAGE("Cannot load kernel32.dll to disable the FS redirector");
				break;
			}

			DisableFsRedirector = 
				(Wow64DisableWow64FsRedirectionHandler)GetProcAddress(
				hKernel32Dll, 
				"Wow64DisableWow64FsRedirection");

			RevertFsRedirector = 
				(Wow64RevertWow64FsRedirectionHandler)GetProcAddress(
				hKernel32Dll, 
				"Wow64RevertWow64FsRedirection");

			if (DisableFsRedirector == NULL || RevertFsRedirector == NULL)
			{
				dwLastError = ERROR_OUTOFMEMORY;
				break;
			}

			if (DisableFsRedirector(&OldFsRedirectorValue) == FALSE)
			{
				dwLastError = GetLastError();
				break;
			}
		}

#endif // _X86_

		// 
		// Find the resource
		//
		HRSRC hRes = FindResource(hInst, ResName, RT_RCDATA);
		if(hRes == NULL)
		{
			dwLastError = GetLastError();
			break;		
		}

		// 
		// Get the size of the resource
		//
		DWORD dwResSize	= SizeofResource(hInst, hRes);
		if(dwResSize == 0)
		{
			dwLastError = GetLastError();
			break;
		}

		TRACE_MESSAGE("The resource has a size of %u bytes", dwResSize);
		// 
		// Load the resource
		//
		HGLOBAL hResGlobal = LoadResource(hInst, hRes);
		if( hResGlobal == NULL)
		{
			dwLastError = GetLastError();
			break;
		}

		// 
		// Get the address of the resource
		// Note: according to MSDN it's not necessary to unlock the resource
		//
		LPVOID lpResMem	= LockResource(hResGlobal);
		if(!hRes)
		{
			dwLastError = GetLastError();
			break;
		}

		hFile = CreateFile(FileName, GENERIC_WRITE, 0, NULL,  OPEN_ALWAYS, 0, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			dwLastError = GetLastError();
			TRACE_MESSAGE("Cannot open file %8.8x", dwLastError);
		}

#ifdef _X86_
		if (bDisableFsRedirector)
		{
			if (RevertFsRedirector(OldFsRedirectorValue) == FALSE)
			{
				dwLastError = GetLastError();
				break;
			}
		}
#endif //_X86_

		if (hFile == INVALID_HANDLE_VALUE)
		{
			break;
		}
	
		//
		// Save the resource to disk
		//
		DWORD writtenBytes = 0;
		do
		{
			DWORD localWrittenBytes;

			if (WriteFile(hFile, lpResMem, dwResSize - writtenBytes, &localWrittenBytes, NULL) == FALSE)
			{
				dwLastError = GetLastError();
				break;
			}
			TRACE_MESSAGE("Written %u bytes", localWrittenBytes);
			writtenBytes += localWrittenBytes;
		}
		while(writtenBytes < dwResSize);
		
	}while(FALSE);

#ifdef _X86_
	if (hKernel32Dll != NULL) FreeLibrary(hKernel32Dll);
#endif //_X86_

	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	if (hFile != INVALID_HANDLE_VALUE && dwLastError != ERROR_SUCCESS) DeleteFileA(FileName);

	if (dwLastError != ERROR_SUCCESS) SetLastError(dwLastError);

	TRACE_EXIT("WoemSaveResourceToDisk");

	return (dwLastError == ERROR_SUCCESS);
}

BOOL WoemGetResource(HINSTANCE hInst, int ResID, LPVOID * lpResMem, LPDWORD dwResSize)
{
	char ResName[100];
	HRSRC hRes;
	BOOL bResult = FALSE;

	TRACE_ENTER("WoemGetResource");

#ifdef STATIC_LIB
	hInst = NULL;
#endif

	StringCchPrintfA(ResName, sizeof(ResName), "#%d", ResID);

	do
	{

		// 
		// Find the resource
		//
		hRes = FindResource(hInst, ResName, RT_RCDATA);
		if(!hRes)
		{
			break;
		}

		// 
		// Get the size of the resource
		//
		*dwResSize	= SizeofResource(hInst, hRes);
		if(dwResSize == 0)
		{
			break;
		}
	
		// 
		// Load the resource
		//
		HGLOBAL hResGlobal = LoadResource(hInst, hRes);
		if(!hResGlobal)
		{
			break;
		}

		// 
		// Get the address of the resource
		// Note: according to MSDN it's not necessary to unlock the resource
		//
		*lpResMem = LockResource(hResGlobal);
		if(!hRes)
		{
			break;
		}
	
		bResult = TRUE;
	}while(FALSE);

	TRACE_EXIT("WoemGetResource");

	return bResult;
}
