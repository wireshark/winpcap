#include <windows.h>
#include <stdio.h>

char* ResourceNames[] =
{
	"packet2k.dll",
	"driver2k.dll"
};

////////////////////////////////////////////////////////////////////
// This function locates one of the binary resources embedded in the
// dll and saves it to disk
////////////////////////////////////////////////////////////////////
BOOL WoemSaveResourceToDisk(HINSTANCE hInst, int ResID, char* FileName)
{
	char ResName[100];
	sprintf(ResName, "#%d", ResID);
	FILE* OutFile;

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
	DWORD dwResSize	= SizeofResource(hInst, hRes);
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
	LPVOID lpResMem	= LockResource(hResGlobal);
	if(!hRes)
	{
		return FALSE;
	}

	//
	// Save the resource to disk
	//
	OutFile = fopen(FileName, "wb");
	if(!OutFile)
	{
		return FALSE;
	}

	if(fwrite(lpResMem,
		1,
		dwResSize,
		OutFile) != dwResSize)
	{
		fclose(OutFile);
		return FALSE;
	}

	fclose(OutFile);

	return TRUE;
}

