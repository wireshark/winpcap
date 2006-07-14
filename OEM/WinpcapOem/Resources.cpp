#include <windows.h>
#include <stdio.h>
#include "WinpcapOem.h"

////////////////////////////////////////////////////////////////////
// This function locates one of the binary resources embedded in the
// dll and saves it to disk
////////////////////////////////////////////////////////////////////
BOOL WoemSaveResourceToDisk(HINSTANCE hInst, int ResID, char* FileName, BOOL bDisableFsRedirector)
{
	char ResName[100];
	FILE* OutFile;
	PVOID OldFsRedirectorValue;
	HMODULE hKernel32Dll = NULL;
	Wow64DisableWow64FsRedirectionHandler DisableFsRedirector;
	Wow64RevertWow64FsRedirectionHandler RevertFsRedirector;

#ifdef STATIC_LIB
	hInst = NULL;
#endif

	if (bDisableFsRedirector)
	{
		//
		// load the FS redirector function dynamically
		//
		hKernel32Dll = LoadLibrary("kernel32.dll");

		if (hKernel32Dll == NULL)
		{
			return FALSE;
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
			FreeLibrary(hKernel32Dll);
			return FALSE;
		}

		if (DisableFsRedirector(&OldFsRedirectorValue) == FALSE)
		{
			FreeLibrary(hKernel32Dll);
			return FALSE;
		}
	}

	sprintf(ResName, "#%d", ResID);
	// 
	// Find the resource
	//
	HRSRC hRes = FindResource(hInst, ResName, RT_RCDATA);
	if(!hRes)
	{
		if (hKernel32Dll != NULL)
		{
			FreeLibrary(hKernel32Dll);
		}
		return FALSE;
	}

	// 
	// Get the size of the resource
	//
	DWORD dwResSize	= SizeofResource(hInst, hRes);
	if(dwResSize == 0)
	{
		if (hKernel32Dll != NULL)
		{
			FreeLibrary(hKernel32Dll);
		}
		return FALSE;
	}

	// 
	// Load the resource
	//
	HGLOBAL hResGlobal = LoadResource(hInst, hRes);
	if(!hResGlobal)
	{
		if (hKernel32Dll != NULL)
		{
			FreeLibrary(hKernel32Dll);
		}
		return FALSE;
	}

	// 
	// Get the address of the resource
	// Note: according to MSDN it's not necessary to unlock the resource
	//
	LPVOID lpResMem	= LockResource(hResGlobal);
	if(!hRes)
	{
		if (hKernel32Dll != NULL)
		{
			FreeLibrary(hKernel32Dll);
		}
		return FALSE;
	}

	//
	// Save the resource to disk
	//
	OutFile = fopen(FileName, "wb");

	if (bDisableFsRedirector)
	{

		if (RevertFsRedirector(OldFsRedirectorValue) == FALSE)
		{
			if (hKernel32Dll != NULL)
			{
				FreeLibrary(hKernel32Dll);
			}
			return FALSE;
		}
	}


	if(!OutFile)
	{
		if (hKernel32Dll != NULL)
		{
			FreeLibrary(hKernel32Dll);
		}

		return FALSE;
	}

	if(fwrite(lpResMem,
		1,
		dwResSize,
		OutFile) != dwResSize)
	{
		fclose(OutFile);

		if (hKernel32Dll != NULL)
		{
			FreeLibrary(hKernel32Dll);
		}

		return FALSE;
	}

	if (hKernel32Dll != NULL)
	{
		FreeLibrary(hKernel32Dll);
	}

	fclose(OutFile);

	return TRUE;
}

BOOL WoemGetResource(HINSTANCE hInst, int ResID, LPVOID * lpResMem, LPDWORD dwResSize)
{
	char ResName[100];
	sprintf(ResName, "#%d", ResID);

#ifdef STATIC_LIB
	hInst = NULL;
#endif

	// 
	// Find the resource
	//
	HRSRC hRes = FindResource(hInst, ResName, RT_RCDATA);
	if(!hRes)
	{
		return FALSE;
	}

	// 
	// Get the size of the resource
	//
	*dwResSize	= SizeofResource(hInst, hRes);
	if(dwResSize == 0)
	{
		return FALSE;
	}

	// 
	// Load the resource
	//
	HGLOBAL hResGlobal = LoadResource(hInst, hRes);
	if(!hResGlobal)
	{
		return FALSE;
	}

	// 
	// Get the address of the resource
	// Note: according to MSDN it's not necessary to unlock the resource
	//
	*lpResMem = LockResource(hResGlobal);
	if(!hRes)
	{
		return FALSE;
	}

	return TRUE;
}
