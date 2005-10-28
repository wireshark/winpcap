#include <stdio.h>
#include <io.h>
#include "Packet32.h"
#include "WpcapNames.h"
#include "WinpcapOem.h"
#include "resource.h"

#ifdef SECURITY
#include "Security.h"
#endif

HANDLE g_hGlobalMutex = NULL;
HANDLE g_hGlobalSemaphore = NULL;
char g_SysDir[MAX_PATH + 16];
char g_GlobalSemaphoreName[MAX_OBJNAME_LEN];
char g_DllFullPath[MAX_PATH + 16];
char g_DriverFullPath[MAX_PATH + 16];
char g_NpfDriverNameId[32];
WCHAR g_NpfDriverNameIdW[32];
HINSTANCE g_DllHandle = NULL;
char g_LastWoemError[PACKET_ERRSTR_SIZE];
BOOL g_InitError = FALSE;
extern BOOL OemActive;
BOOL g_IsProcAuthorized = FALSE;

////////////////////////////////////////////////////////////////////
// DLL Entry point
////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HINSTANCE Dllh, DWORD Reason, LPVOID lpReserved)
{
	switch(Reason)
    {
		
	case DLL_PROCESS_ATTACH:
		
		g_DllHandle = Dllh;
		
#ifdef SECURITY
		//
		// the version with security enabled doesn't need to be activated with PacketStartOem()
		//
		OemActive = TRUE;
#endif
		
		break;
		
	case DLL_PROCESS_DETACH:
		
		WoemLeaveDll();
		
		break;
		
	default:
		break;
    }
	
    return TRUE;
}

////////////////////////////////////////////////////////////////////
// Function called when a process loads the dll.
// If this is the first time the dll is loaded, we start the driver
// service
////////////////////////////////////////////////////////////////////
PCHAR getObjectName(PCHAR systemObjectName, UINT strlen)
{
	OSVERSIONINFO osVer;
	char TmpName[MAX_OBJNAME_LEN];

	ZeroMemory(&osVer,sizeof(OSVERSIONINFO));
	osVer.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);

	if (GetVersionEx(&osVer)==0) 
	{
		return NULL;
	}

	//Version prior to 2000 does not accept any / character for system object name
	//and they does not understand it
	if (osVer.dwMajorVersion > 4)
	{
		_snprintf(TmpName, strlen, "Global\\%s", systemObjectName);
		memcpy(systemObjectName, TmpName, strlen);
	}

	return systemObjectName;
}

////////////////////////////////////////////////////////////////////
// Function called when a process loads the dll.
// If this is the first time the dll is loaded, we start the driver
// service
////////////////////////////////////////////////////////////////////
BOOL WoemEnterDll(HINSTANCE DllHandle)
{
	SECURITY_DESCRIPTOR sd;
	SECURITY_ATTRIBUTES sa;
	BOOLEAN retry;
	LONG lold;
	char ObjName[MAX_OBJNAME_LEN];
	UINT i;
	char SvcDesc[MAX_PATH + 16];
	char SvcBin[MAX_PATH + 16];
	OSVERSIONINFO osVer;
    HRESULT hr;
#ifdef SECURITY
	DWORD Result;
#endif 

#ifdef SECURITY
	if (!g_IsProcAuthorized)
	{
	//
		// the version with security enabled doesn't need to be activated with PacketStartOem()
		//
		OemActive = TRUE;
		Result = WoemGetCurrentProcessAuthorization(g_LastWoemError);
		
		if (Result == WOEM_PROC_AUTHORIZATION_FAILURE)
		{
			g_InitError = TRUE;
			g_IsProcAuthorized = FALSE;
		}
		else
			if (Result == WOEM_PROC_AUTHORIZATION_OK)
			{
				g_IsProcAuthorized = TRUE;
			}
			else
			{
				g_IsProcAuthorized = FALSE;
				MessageBox(NULL, "This version of WinPcap OEM can be only run in conjunction with CACE Technologies Network Toolkit. This program is not recognized as part of The Network Toolkit, and therefore WinPcap OEM will not work.", "Error", MB_ICONERROR);
				return FALSE;
			}
	}		
#endif

#ifndef STATIC_LIB
	//
	// This should REALLY never happen
	//
	if(!g_DllHandle)
	{
		_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "NULL DLL Handle");
		WoemReportError();
		return FALSE;
	}
#endif

	if(g_hGlobalMutex)
	{
		//
		// This should never happen, but better to be sure...
		//
		_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "Double initialization");
		WoemReportError();
		return FALSE;
	}

	//
	// Create a Security Descriptor with NULL DACL to turn off all security checks.
	// This allows to share the Mutex among different users.
	//
	InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = &sd;
	sa.nLength = sizeof(sa);

	//
	// Get the MUTEX
	//
	_snprintf(ObjName, MAX_OBJNAME_LEN, MUTEX_NAME);
	g_hGlobalMutex = CreateMutex(&sa, FALSE, getObjectName(ObjName, MAX_OBJNAME_LEN));
	if (g_hGlobalMutex == NULL)
	{
		if (GetLastError()==5) 
		{
			_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "not enough priviles to start the packet driver");
		}
		
		_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "unable to create the global mutex");

		WoemReportError();

		return FALSE;
	}

	//
	// Try to acquire the ownership of the mutex
	//
	retry = TRUE;
	
	do 
	{
		DWORD result=WaitForSingleObject(g_hGlobalMutex,10000);
		switch(result) 
		{
		case WAIT_FAILED:
			
			ReleaseMutex(g_hGlobalMutex);
			
			if(g_hGlobalMutex!=0)
			{
				CloseHandle(g_hGlobalMutex);
				g_hGlobalMutex = NULL;
				
			}
			if (g_hGlobalSemaphore!=0)
			{
				CloseHandle(g_hGlobalSemaphore);
				g_hGlobalSemaphore = NULL;
			}
			
			return FALSE;
			
		case WAIT_TIMEOUT:
			
			_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "Timeout on the global mutex");
			WoemReportError();
			
			break;
			
		default:				
			retry = FALSE;
			break;
		}
	} while (retry);
	
	//
	// Create a Security Descriptor with NULL DACL to turn off all security checks.
	// This allows to share the Semaphore among different users.
	//
	InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = &sd;
	sa.nLength =sizeof(sa);
	
	//
	// Get the semaphore
	//
	_snprintf(g_GlobalSemaphoreName, MAX_OBJNAME_LEN, SEMAPHORE_NAME);
	g_hGlobalSemaphore = CreateSemaphore(&sa, 0, MAX_VALUE_SEMAPHORE, getObjectName(g_GlobalSemaphoreName, MAX_OBJNAME_LEN));
	if (g_hGlobalSemaphore == NULL) 
	{
		if (GetLastError()==5) 
		{
			_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "not enough priviles to start the packet driver");
		}
		else
		{			
			_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "unable to create the global semaphore");
		}
		
		ReleaseMutex(g_hGlobalMutex);
		
		WoemReportError();

		if(g_hGlobalMutex!=0)
		{
			CloseHandle(g_hGlobalMutex);
			g_hGlobalMutex = NULL;
			
		}
		if (g_hGlobalSemaphore!=0)
		{
			CloseHandle(g_hGlobalSemaphore);
			g_hGlobalSemaphore = NULL;
		}
		
		return FALSE;
	}
	
	//
	// Check the value of the semaphore
	//
	if(ReleaseSemaphore(g_hGlobalSemaphore, 1, &lold)==FALSE)
	{
		_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "unable to release the semaphore");
		
		ReleaseMutex(g_hGlobalMutex);
		
		WoemReportError();

		if(g_hGlobalMutex!=0)
		{
			CloseHandle(g_hGlobalMutex);
			g_hGlobalMutex = NULL;
			
		}
		if (g_hGlobalSemaphore!=0)
		{
			CloseHandle(g_hGlobalSemaphore);
			g_hGlobalSemaphore = NULL;
		}
		
		return FALSE;
	}
	
	if (lold==0)
	{		
		//
		// IF WE ARE HERE, THE DLL IS BEING LOADED FOR THE FIRST TIME.
		//
		
		//
		// Get the OS we're running on
		//
		ZeroMemory(&osVer, sizeof(OSVERSIONINFO));
		osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		
		if (GetVersionEx(&osVer)==0) 
		{
			_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to determine OS version");
	
			ReleaseMutex(g_hGlobalMutex);
			
			WoemReportError();
	
			if(g_hGlobalMutex!=0)
			{
				CloseHandle(g_hGlobalMutex);
				g_hGlobalMutex = NULL;

			}
			if (g_hGlobalSemaphore!=0)
			{
				CloseHandle(g_hGlobalSemaphore);
				g_hGlobalSemaphore = NULL;
			}
			
			return FALSE;
		}
		
		//
		// Create the WinPcap global registry key
		//
		if(!WoemCreateNameRegistryEntries())
		{
			_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "unable to create registry entries. Administrator provileges are required for this operation");

			ReleaseMutex(g_hGlobalMutex);
			
			WoemReportError();

			if(g_hGlobalMutex!=0)
			{
				CloseHandle(g_hGlobalMutex);
				g_hGlobalMutex = NULL;

			}
			if (g_hGlobalSemaphore!=0)
			{
				CloseHandle(g_hGlobalSemaphore);
				g_hGlobalSemaphore = NULL;
			}
			
			return FALSE;
		}

		if(!WoemCreateBinaryNames())
		{
			_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "unable to create binary names");
	
			ReleaseMutex(g_hGlobalMutex);

			WoemReportError();
			
			if(g_hGlobalMutex!=0)
			{
				CloseHandle(g_hGlobalMutex);
				g_hGlobalMutex = NULL;

			}
			if (g_hGlobalSemaphore!=0)
			{
				CloseHandle(g_hGlobalSemaphore);
				g_hGlobalSemaphore = NULL;
			}
			
			return FALSE;
		}
		
		if(check_if_service_is_running(g_NpfDriverNameId) == 0)
		{
			//
			// If we are here and the service is running, it's almost surely the result 
			// of some mess. We try to cleanup.
			//
			delete_service(g_NpfDriverNameId);
		}
		
		//
		// Now perform system-dependent operations
		//
		if(osVer.dwMajorVersion == 4)
		{
			//
			// Windows NT4
			//

			//
			// Extract packet.dll to disk
			//
			if(!WoemSaveResourceToDisk(g_DllHandle, IDP_DLLNT, g_DllFullPath))
			{
				_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to copy the WinPcap OEM files. Administrative privileges are required for this operation.", g_DllFullPath);

				ReleaseMutex(g_hGlobalMutex);

				WoemReportError();
				
				if(g_hGlobalMutex!=0)
				{
					CloseHandle(g_hGlobalMutex);
					g_hGlobalMutex = NULL;
					
				}
				if (g_hGlobalSemaphore!=0)
				{
					CloseHandle(g_hGlobalSemaphore);
					g_hGlobalSemaphore = NULL;
				}
				
				return FALSE;
			}
			
			//
			// Extract the driver to disk
			//
			if(!WoemSaveResourceToDisk(g_DllHandle, IDP_DRINT, g_DriverFullPath))
			{
				_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to copy the WinPcap OEM files. Administrative privileges are required for this operation.", g_DllFullPath);

				_unlink(g_DllFullPath);
				
				ReleaseMutex(g_hGlobalMutex);

				WoemReportError();
				
				if(g_hGlobalMutex!=0)
				{
					CloseHandle(g_hGlobalMutex);
					g_hGlobalMutex = NULL;
					
				}
				if (g_hGlobalSemaphore!=0)
				{
					CloseHandle(g_hGlobalSemaphore);
					g_hGlobalSemaphore = NULL;
				}
				
				return FALSE;
			}
		}
		else if(osVer.dwMajorVersion == 5)
		{
			//
			// Windows 2000, XP, 2003
			//
			
			//
			// Install Netmon
			//
			if(check_if_service_is_present("nm") != 0)
			{
				hr = HrInstallNetMonProtocol();
				if (hr != S_OK)
				{
					_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "Warning: unable to load the netmon driver, ndiswan captures will not be available");
					WoemReportError();
				}
			}
			else
			{
				// printf("netmon already here!!!\n");
			}
			
			//
			// Extract packet.dll to disk
			//
			if(!WoemSaveResourceToDisk(g_DllHandle, IDP_DLL2K, g_DllFullPath))
			{
				_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to copy the WinPcap OEM files. Administrative privileges are required for this operation.");

				ReleaseMutex(g_hGlobalMutex);
				
				WoemReportError();
				
				if(g_hGlobalMutex!=0)
				{
					CloseHandle(g_hGlobalMutex);
					g_hGlobalMutex = NULL;
					
				}
				if (g_hGlobalSemaphore!=0)
				{
					CloseHandle(g_hGlobalSemaphore);
					g_hGlobalSemaphore = NULL;
				}
				
				return FALSE;
			}
			
			//
			// Extract the driver to disk
			//
			if(!WoemSaveResourceToDisk(g_DllHandle, IDP_DRI2K, g_DriverFullPath))
			{
				_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to copy the WinPcap OEM files. Administrative privileges are required for this operation.", g_DllFullPath);

				_unlink(g_DllFullPath);
				
				ReleaseMutex(g_hGlobalMutex);
				
				WoemReportError();

				if(g_hGlobalMutex!=0)
				{
					CloseHandle(g_hGlobalMutex);
					g_hGlobalMutex = NULL;
					
				}
				if (g_hGlobalSemaphore!=0)
				{
					CloseHandle(g_hGlobalSemaphore);
					g_hGlobalSemaphore = NULL;
				}
				
				return FALSE;
			}
		}
		else if(osVer.dwMajorVersion == 6)
		{
			//
			// Windows Vista
			//
			
			//
			// Extract packet.dll to disk
			//
			if(!WoemSaveResourceToDisk(g_DllHandle, IDP_DLL2K, g_DllFullPath))
			{
				_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to copy the WinPcap OEM files. Administrative privileges are required for this operation.", g_DllFullPath);

				ReleaseMutex(g_hGlobalMutex);

				WoemReportError();
				
				if(g_hGlobalMutex!=0)
				{
					CloseHandle(g_hGlobalMutex);
					g_hGlobalMutex = NULL;
					
				}
				if (g_hGlobalSemaphore!=0)
				{
					CloseHandle(g_hGlobalSemaphore);
					g_hGlobalSemaphore = NULL;
				}
				
				return FALSE;
			}
			
			//
			// Extract the driver to disk
			//
			if(!WoemSaveResourceToDisk(g_DllHandle, IDP_DRI2K, g_DriverFullPath))
			{
				_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to copy the WinPcap OEM files. Administrative privileges are required for this operation.");

				_unlink(g_DllFullPath);

				WoemReportError();
				
				ReleaseMutex(g_hGlobalMutex);
				
				if(g_hGlobalMutex!=0)
				{
					CloseHandle(g_hGlobalMutex);
					g_hGlobalMutex = NULL;
					
				}
				if (g_hGlobalSemaphore!=0)
				{
					CloseHandle(g_hGlobalSemaphore);
					g_hGlobalSemaphore = NULL;
				}
				
				return FALSE;
			}
			
		}

		//
		// Create the driver service
		//		
		_snprintf(SvcDesc, 
			sizeof(SvcDesc) - 1, 
			"WinPcap Packet Driver (%s)",
			g_NpfDriverNameId);
		
		_snprintf(SvcBin, 
			sizeof(SvcDesc) - 1, 
			"system32\\drivers\\%s.sys",
			g_NpfDriverNameId);
		
		if(create_driver_service(g_NpfDriverNameId, 
			SvcDesc, 
			SvcBin) == -1)
		{
			_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "unable to create the packet driver service");

			_unlink(g_DllFullPath);
			_unlink(g_DriverFullPath);
			
			ReleaseMutex(g_hGlobalMutex);

			WoemReportError();
			
			if(g_hGlobalMutex!=0)
			{
				CloseHandle(g_hGlobalMutex);
				g_hGlobalMutex = NULL;
				
			}
			if (g_hGlobalSemaphore!=0)
			{
				CloseHandle(g_hGlobalSemaphore);
				g_hGlobalSemaphore = NULL;
			}
			
			return FALSE;
		}

		//
		// Load the driver
		//
		if(start_service(g_NpfDriverNameId) == -1)
		{
			_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "unable to start the packet driver service");

			delete_service(g_NpfDriverNameId);
			_unlink(g_DllFullPath);
			_unlink(g_DriverFullPath);
			
			ReleaseMutex(g_hGlobalMutex);

			WoemReportError();
			
			if(g_hGlobalMutex!=0)
			{
				CloseHandle(g_hGlobalMutex);
				g_hGlobalMutex = NULL;
				
			}
			if (g_hGlobalSemaphore!=0)
			{
				CloseHandle(g_hGlobalSemaphore);
				g_hGlobalSemaphore = NULL;
			}
			
			return FALSE;
		}

		//
		// Wait until the service is running
		//
		i = 0;
		while(TRUE)
		{
			if(check_if_service_is_running(g_NpfDriverNameId) == 0)
			{
				break;
			}
			
			i++;
			if(i == 300)
			{
				_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "timeout while starting the packet driver");
	
				delete_service(g_NpfDriverNameId);
				_unlink(g_DllFullPath);
				_unlink(g_DriverFullPath);
				
				ReleaseMutex(g_hGlobalMutex);

				WoemReportError();
				
				if(g_hGlobalMutex!=0)
				{
					CloseHandle(g_hGlobalMutex);
					g_hGlobalMutex = NULL;
					
				}
				if (g_hGlobalSemaphore!=0)
				{
					CloseHandle(g_hGlobalSemaphore);
					g_hGlobalSemaphore = NULL;
				}
				
				return FALSE;
				
			}

			Sleep(100);
		}
		
		//
		// We've loaded the driver, we can delete its binary
		//
		_unlink(g_DriverFullPath);
	}
		
	//
	// Create the names for the binaries
	//
	if(!WoemCreateBinaryNames())
	{
		_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "unable to create binary names");

		delete_service(g_NpfDriverNameId);
		_unlink(g_DllFullPath);
		_unlink(g_DriverFullPath);
		
		ReleaseMutex(g_hGlobalMutex);

		WoemReportError();
		
		if(g_hGlobalMutex!=0)
		{
			CloseHandle(g_hGlobalMutex);
			g_hGlobalMutex = NULL;
			
		}
		if (g_hGlobalSemaphore!=0)
		{
			CloseHandle(g_hGlobalSemaphore);
			g_hGlobalSemaphore = NULL;
		}
		
		return FALSE;
	}

	//
	// Load packet.dll and intercept all the calls
	//
	if(!LoadPacketDll(g_DllFullPath))
	{
		_snprintf(g_LastWoemError, PACKET_ERRSTR_SIZE - 1, "unable to load packet.dll");

		delete_service(g_NpfDriverNameId);
		_unlink(g_DllFullPath);
		_unlink(g_DriverFullPath);
		
		ReleaseMutex(g_hGlobalMutex);

		WoemReportError();
		
		if(g_hGlobalMutex!=0)
		{
			CloseHandle(g_hGlobalMutex);
			g_hGlobalMutex = NULL;
			
		}
		if (g_hGlobalSemaphore!=0)
		{
			CloseHandle(g_hGlobalSemaphore);
			g_hGlobalSemaphore = NULL;
		}
		
		return FALSE;
	}
	
	//
	// Register an unload handler with packet.dll
	//
#ifdef STATIC_LIB
	RegisterPacketUnloadHandler(WoemLeaveDll);
#endif // STATIC_LIB

	//
	// Schedule the deletion of the dll. It will go away at the next reboot
	//
	DeleteDll(g_DllFullPath);	
	
	//
	// Done. We can release the MUTEX
	//
	ReleaseMutex(g_hGlobalMutex);

	return TRUE;
}

////////////////////////////////////////////////////////////////////
// Function called when a process loads the dll.
////////////////////////////////////////////////////////////////////
BOOL WoemLeaveDll()
{
	DWORD result;
	BOOLEAN retry;

	//
	// Try to acquire the ownership of the mutex
	//
	retry = TRUE;
	do 
	{
		DWORD result=WaitForSingleObject(g_hGlobalMutex, 10000);
		switch(result) 
		{
		case WAIT_FAILED:
			
			if(g_hGlobalMutex!=0)
			{
				CloseHandle(g_hGlobalMutex);
				g_hGlobalMutex = NULL;

			}
			if (g_hGlobalSemaphore!=0)
			{
				CloseHandle(g_hGlobalSemaphore);
				g_hGlobalSemaphore = NULL;
			}

			return FALSE;
			
		case WAIT_TIMEOUT:
	
			break;
			
		default:				
			retry = FALSE;
			break;
		}
	} while (retry);
	
	
	// 
	// See if the semaphore count is 1
	//
	if(WaitForSingleObject(g_hGlobalSemaphore, 0) == WAIT_FAILED)
	{
		ReleaseMutex(g_hGlobalMutex);
		
		if(g_hGlobalMutex!=0)
		{
			CloseHandle(g_hGlobalMutex);
			g_hGlobalMutex = NULL;
			
		}
		if (g_hGlobalSemaphore!=0)
		{
			CloseHandle(g_hGlobalSemaphore);
			g_hGlobalSemaphore = NULL;
		}
				
		return FALSE;
	}
	
	//
	// Now check if we would block on the semaphore
	//
	result = WaitForSingleObject(g_hGlobalSemaphore, 0);
	
	switch (result) 
	{
	case WAIT_TIMEOUT:

		//
		// IF WE ARE HERE, THE DLL IS BEING UNLOADED FOR THE LAST TIME.
		//

		delete_service(g_NpfDriverNameId);

		break;

	case WAIT_FAILED: 

		ReleaseMutex(g_hGlobalMutex);
		
		if(g_hGlobalMutex!=0)
		{
			CloseHandle(g_hGlobalMutex);
			g_hGlobalMutex = NULL;
			
		}
		if (g_hGlobalSemaphore!=0)
		{
			CloseHandle(g_hGlobalSemaphore);
			g_hGlobalSemaphore = NULL;
		}
		
		return FALSE;
		
	case WAIT_OBJECT_0: 
		if (ReleaseSemaphore(g_hGlobalSemaphore,1,NULL) == FALSE)
		{
			ReleaseMutex(g_hGlobalMutex);
			
			if(g_hGlobalMutex!=0)
			{
				CloseHandle(g_hGlobalMutex);
				g_hGlobalMutex = NULL;
				
			}
			if (g_hGlobalSemaphore!=0)
			{
				CloseHandle(g_hGlobalSemaphore);
				g_hGlobalSemaphore = NULL;
			}
						
			return FALSE;
		}
		
		break;
	}
	
	// 
	// Release the MUTEX
	//
	ReleaseMutex(g_hGlobalMutex);

	//
	// We're done, close the handles
	//
	if(g_hGlobalMutex!=0)
	{
		CloseHandle(g_hGlobalMutex);
		g_hGlobalMutex = NULL;

	}
	if (g_hGlobalSemaphore!=0)
	{
		CloseHandle(g_hGlobalSemaphore);
		g_hGlobalSemaphore = NULL;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////
// Create the temporary registry key with the name that all the  
// WinPcap components will use
////////////////////////////////////////////////////////////////////
BOOL WoemCreateNameRegistryEntries()
{
	HKEY WinpcapKey;
	DWORD DriverId = 0;
	DWORD Type;
	DWORD Len = sizeof(DriverId);
	char KeyValBuf[MAX_WINPCAP_KEY_CHARS];
	WCHAR KeyValBufW[MAX_WINPCAP_KEY_CHARS];

	//
	// First, check if the WinPcap global key is already present and
	// if yes retrieve the id of the last loaded instance
	//
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		WINPCAP_INSTANCE_KEY,
		0,
		KEY_ALL_ACCESS,
		&WinpcapKey) == ERROR_SUCCESS)
	{
		//
		// Key present, check the ID
		//
		if(RegQueryValueEx(WinpcapKey,
			"npf_driver_id",
			NULL,
			&Type,
			(LPBYTE)&DriverId,
			&Len) != ERROR_SUCCESS)	// We include two bytes for string termination
		{
			//
			// Key present but ID not present: this is the result of some mess
			//
			DriverId = 0;
		}		
	}

	//
	// Do a cleanup, just to be sure
	//
	RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);

	//
	// Create the global WinPcap key
	//
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		WINPCAP_INSTANCE_KEY,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&WinpcapKey,
		0) != ERROR_SUCCESS)
	{
		return FALSE;
	}
	
	//
	// Increase the ID and write it into the registry
	//
	DriverId++;
	
	if(DriverId > 99)
		DriverId = 0;

	if(RegSetValueEx(WinpcapKey,
		"npf_driver_id",
		0, 
		REG_DWORD, 
		(LPBYTE)&DriverId, 
		sizeof(DriverId)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	//
	// Created the strings that we'll use to build the registry keys values
	// and load the components
	//
/* XXX this is the totally dynamic version!

	_snprintf(NpfDrNameWhId, 
		sizeof(NpfDrNameWhId) - 1, 
		"%s%.2d",
		NPF_DRIVER_NAME,
		DriverId);

	_snwprintf(NpfDrNameWhIdW, 
		sizeof(NpfDrNameWhIdW) / sizeof(WCHAR) - 1, 
		L"%ws%.2d",
		NPF_DRIVER_NAME_WIDECHAR,
		DriverId);	
*/	
	_snprintf(g_NpfDriverNameId, 
		sizeof(g_NpfDriverNameId) - 1, 
		"%s",
		NPF_DRIVER_NAME);

	_snwprintf(g_NpfDriverNameIdW, 
		sizeof(g_NpfDriverNameIdW) / sizeof(WCHAR) - 1, 
		L"%ws",
		NPF_DRIVER_NAME_WIDECHAR);	

	//
	// Add subkeys with the strings
	//

	// npf_driver_name

	memset(KeyValBuf, sizeof(KeyValBuf), 0);
	memset(KeyValBufW, sizeof(KeyValBufW), 0);

	if(RegSetValueEx(WinpcapKey, 
		"npf_driver_name", 
		0, 
		REG_BINARY, 
		(LPBYTE)g_NpfDriverNameId,
		strlen(g_NpfDriverNameId) + 1) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}
	
	// npf_driver_name_widechar

	memset(KeyValBuf, sizeof(KeyValBuf), 0);
	memset(KeyValBufW, sizeof(KeyValBufW), 0);

	if(RegSetValueEx(WinpcapKey, 
		"npf_driver_name_widechar", 
		0,
		REG_BINARY,
		(LPBYTE)g_NpfDriverNameIdW,
		(wcslen(g_NpfDriverNameIdW) + 1) * sizeof(WCHAR)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	// npf_service_desc_widechar

	memset(KeyValBuf, sizeof(KeyValBuf), 0);
	memset(KeyValBufW, sizeof(KeyValBufW), 0);

	_snwprintf(KeyValBufW, 
		sizeof(KeyValBufW) / sizeof(WCHAR) - 1, 
		L"%ws (%ws)",
		NPF_SERVICE_DESC_WIDECHAR,
		g_NpfDriverNameIdW);

	if(RegSetValueEx(WinpcapKey, 
		"npf_service_desc_widechar", 
		0,
		REG_BINARY,
		(LPBYTE)KeyValBufW,
		(wcslen(KeyValBufW) + 1) * sizeof(WCHAR)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	// npf_service_registry_location_widechar

	memset(KeyValBuf, sizeof(KeyValBuf), 0);
	memset(KeyValBufW, sizeof(KeyValBufW), 0);

	_snwprintf(KeyValBufW, 
		sizeof(KeyValBufW) / sizeof(WCHAR) - 1, 
		L"SYSTEM\\CurrentControlSet\\Services\\%ws",
		g_NpfDriverNameIdW);

	if(RegSetValueEx(WinpcapKey, 
		"npf_service_registry_location_widechar", 
		0,
		REG_BINARY,
		(LPBYTE)KeyValBufW,
		(wcslen(KeyValBufW) + 1) * sizeof(WCHAR)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	// npf_complete_service_registry_location

	memset(KeyValBuf, sizeof(KeyValBuf), 0);
	memset(KeyValBufW, sizeof(KeyValBufW), 0);

	_snprintf(KeyValBuf, 
		sizeof(KeyValBuf) / sizeof(CHAR) - 1, 
		"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\%s",
		g_NpfDriverNameId);

	if(RegSetValueEx(WinpcapKey, 
		"npf_complete_service_registry_location", 
		0,
		REG_BINARY,
		(LPBYTE)KeyValBuf,
		strlen(KeyValBuf) + 1) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	// npf_driver_path_widechar

	if(RegSetValueEx(WinpcapKey, 
		"npf_driver_path_widechar", 
		0,
		REG_BINARY,
		(LPBYTE)NPF_DRIVER_PATH_WIDECHAR,
		sizeof(NPF_DRIVER_PATH_WIDECHAR)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	// npf_driver_binary_widechar

	memset(KeyValBuf, sizeof(KeyValBuf), 0);
	memset(KeyValBufW, sizeof(KeyValBufW), 0);

	_snwprintf(KeyValBufW, 
		sizeof(KeyValBufW) / sizeof(WCHAR) - 1, 
		L"%ws.sys",
		g_NpfDriverNameIdW);

	if(RegSetValueEx(WinpcapKey, 
		"npf_driver_binary_widechar", 
		0,
		REG_BINARY,
		(LPBYTE)KeyValBufW,
		(wcslen(KeyValBufW) + 1) * sizeof(WCHAR)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	// npf_device_names_prefix

	memset(KeyValBuf, sizeof(KeyValBuf), 0);
	memset(KeyValBufW, sizeof(KeyValBufW), 0);

	_snprintf(KeyValBuf, 
		sizeof(KeyValBuf) / sizeof(CHAR) - 1, 
		"%s_",
		g_NpfDriverNameId);

	if(RegSetValueEx(WinpcapKey, 
		"npf_device_names_prefix", 
		0,
		REG_BINARY,
		(LPBYTE)KeyValBuf,
		strlen(KeyValBuf) + 1) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	// npf_device_names_prefix_widechar

	memset(KeyValBuf, sizeof(KeyValBuf), 0);
	memset(KeyValBufW, sizeof(KeyValBufW), 0);

	_snwprintf(KeyValBufW, 
		sizeof(KeyValBufW) / sizeof(WCHAR) - 1, 
		L"%ws_",
		g_NpfDriverNameIdW);

	if(RegSetValueEx(WinpcapKey, 
		"npf_device_names_prefix_widechar", 
		0,
		REG_BINARY,
		(LPBYTE)KeyValBufW,
		(wcslen(KeyValBufW) + 1) * sizeof(WCHAR)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	// npf_user_events_names

	memset(KeyValBuf, sizeof(KeyValBuf), 0);
	memset(KeyValBufW, sizeof(KeyValBufW), 0);

	_snwprintf(KeyValBufW, 
		sizeof(KeyValBufW) / sizeof(WCHAR) - 1, 
		L"%ws0000000000",
		g_NpfDriverNameIdW);

	if(RegSetValueEx(WinpcapKey, 
		"npf_user_events_names", 
		0,
		REG_BINARY,
		(LPBYTE)KeyValBufW,
		(wcslen(KeyValBufW) + 1) * sizeof(WCHAR)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	// npf_kernel_events_names

	memset(KeyValBuf, sizeof(KeyValBuf), 0);
	memset(KeyValBufW, sizeof(KeyValBufW), 0);

	_snwprintf(KeyValBufW, 
		sizeof(KeyValBufW) / sizeof(WCHAR) - 1, 
		L"\\BaseNamedObjects\\%ws0000000000\0",
		g_NpfDriverNameIdW);

	if(RegSetValueEx(WinpcapKey, 
		"npf_kernel_events_names", 
		0,
		REG_BINARY,
		(LPBYTE)KeyValBufW,
		(wcslen(KeyValBufW) + 1) * sizeof(WCHAR)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	// fake_ndiswan_adapter_name

	memset(KeyValBuf, sizeof(KeyValBuf), 0);
	memset(KeyValBufW, sizeof(KeyValBufW), 0);

	_snprintf(KeyValBuf, 
		sizeof(KeyValBuf) / sizeof(CHAR) - 1, 
		"\\Device\\%s_GenericDialupAdapter",
		g_NpfDriverNameId);

	if(RegSetValueEx(WinpcapKey, 
		"fake_ndiswan_adapter_name", 
		0,
		REG_BINARY,
		(LPBYTE)KeyValBuf,
		strlen(KeyValBuf) + 1) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	// fake_ndiswan_adapter_description

	if(RegSetValueEx(WinpcapKey, 
		"fake_ndiswan_adapter_description", 
		0,
		REG_BINARY,
		(LPBYTE)FAKE_NDISWAN_ADAPTER_DESCRIPTION,
		sizeof(FAKE_NDISWAN_ADAPTER_DESCRIPTION)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	// npf_driver_complete_widechar

	memset(KeyValBuf, sizeof(KeyValBuf), 0);
	memset(KeyValBufW, sizeof(KeyValBufW), 0);

	_snwprintf(KeyValBufW, 
		sizeof(KeyValBufW) / sizeof(WCHAR) - 1, 
		L"system32\\drivers\\%ws.sys",
		g_NpfDriverNameIdW);

	if(RegSetValueEx(WinpcapKey,
		"npf_driver_complete_widechar", 
		0,
		REG_BINARY,
		(LPBYTE)KeyValBufW,
		(wcslen(KeyValBufW) + 1) * sizeof(WCHAR)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}
	
	// npf_driver_complete_driver_prefix

	memset(KeyValBuf, sizeof(KeyValBuf), 0);
	memset(KeyValBufW, sizeof(KeyValBufW), 0);

	_snprintf(KeyValBuf,
		sizeof(KeyValBuf) / sizeof(CHAR) - 1, 
		"\\Device\\%s_",
		g_NpfDriverNameId);

	if(RegSetValueEx(WinpcapKey,
		"npf_driver_complete_driver_prefix",
		0,
		REG_BINARY,
		(LPBYTE)KeyValBuf,
		strlen(KeyValBuf) + 1) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	RegCloseKey(WinpcapKey);	

	return TRUE;
}

////////////////////////////////////////////////////////////////////
// Create the binaries names according to the name we have 
////////////////////////////////////////////////////////////////////
BOOL WoemCreateBinaryNames()
{
	UINT GsdRes;

	//
	// Get the location of the system folder to create the complete paths
	//
	GsdRes = GetSystemDirectory(g_SysDir, sizeof(g_SysDir));
	if(GsdRes == 0 || GsdRes == sizeof(g_SysDir))
	{
		return FALSE;
	}

/* XXX this is the totally dynamic version!
	//
	// First, check if the WinPcap global key is already present and
	// if yes retrieve the id of the last loaded instance
	//
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		WINPCAP_INSTANCE_KEY,
		0,
		KEY_ALL_ACCESS,
		&WinpcapKey) == ERROR_SUCCESS)
	{
		//
		// Key present, check the ID
		//
		if(RegQueryValueEx(WinpcapKey,
			"npf_driver_id",
			NULL,
			&Type,
			(LPBYTE)&DriverId,
			&Len) != ERROR_SUCCESS)	// We include two bytes for string termination
		{
			//
			// Key present but ID not present: this is the result of some mess
			//
			DriverId = 0;
		}		
	}

	//
	// Created the strings that we'll use to build the registry keys values
	// and load the components
	//

	_snprintf(NpfDrNameWhId, 
		sizeof(NpfDrNameWhId) - 1, 
		"%s%.2d",
		NPF_DRIVER_NAME,
		DriverId);

	_snwprintf(NpfDrNameWhIdW, 
		sizeof(NpfDrNameWhIdW) / sizeof(WCHAR) - 1, 
		L"%ws%.2d",
		NPF_DRIVER_NAME_WIDECHAR,
		DriverId);	
*/	
	_snprintf(g_NpfDriverNameId, 
		sizeof(g_NpfDriverNameId) - 1, 
		"%s",
		NPF_DRIVER_NAME);

	_snwprintf(g_NpfDriverNameIdW, 
		sizeof(g_NpfDriverNameIdW) / sizeof(WCHAR) - 1, 
		L"%ws",
		NPF_DRIVER_NAME_WIDECHAR);	
	
	_snprintf(g_DllFullPath, 
		sizeof(g_DllFullPath) - 1, 
		"%s\\%swoem.tmp", 
		g_SysDir, 
		g_NpfDriverNameId);

	_snprintf(g_DriverFullPath, 
		sizeof(g_DriverFullPath) - 1, 
		"%s\\drivers\\%s.sys", 
		g_SysDir, 
		g_NpfDriverNameId);

	return TRUE;
}

////////////////////////////////////////////////////////////////////
// Delete the temporary registry key with the name that all the  
// WinPcap components will use
////////////////////////////////////////////////////////////////////
BOOL WoemDeleteNameRegistryEntries()
{
	if(RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY) != ERROR_SUCCESS)
		return FALSE;
	else
		return TRUE;
}