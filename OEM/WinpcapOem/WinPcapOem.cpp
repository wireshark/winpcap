#include <stdio.h>
#include <io.h>
#include "Packet32.h"
#include "WpcapNames.h"
#include "WinpcapOem.h"
#include "resource.h"
#include "WoemDebug.h"

#ifdef SECURITY
#include "Security.h"
#endif

HANDLE g_hGlobalMutex = NULL;
HANDLE g_hGlobalSemaphore = NULL;
char g_GlobalSemaphoreName[MAX_OBJNAME_LEN];
char g_DllFullPath[MAX_PATH + 16];
char g_DriverFullPath[MAX_PATH + 16];
HINSTANCE g_DllHandle = NULL;
char g_LastWoemError[PACKET_ERRSTR_SIZE];
volatile BOOL g_InitError = FALSE;
BOOL g_IsProcAuthorized = FALSE;

CRITICAL_SECTION g_CritSectionProtectingWoemEnterDll;

////////////////////////////////////////////////////////////////////
// DLL Entry point
////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HINSTANCE Dllh, DWORD Reason, LPVOID lpReserved)
{
	switch(Reason)
    {

	case DLL_PROCESS_ATTACH:
		
		g_DllHandle = Dllh;
		::InitializeCriticalSection(&::g_CritSectionProtectingWoemEnterDll);
		
#ifdef SECURITY
		//
		// the version with security enabled doesn't need to be activated with PacketStartOem()
		//
		g_OemActive = TRUE;
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


static BOOL WoemEnterDllInternal(HINSTANCE DllHandle, char *WoemErrorString);


BOOL WoemEnterDll(HINSTANCE DllHandle, char *WoemErrorString)
{
	BOOL returnValue = TRUE;

	TRACE_ENTER("WoemEnterDll");
	
	::EnterCriticalSection(&::g_CritSectionProtectingWoemEnterDll);

	if (g_StillToInit)
	{
		returnValue = WoemEnterDllInternal(DllHandle, WoemErrorString);
		if (returnValue == TRUE)
		{
			g_StillToInit = FALSE;
		}
	}

	::LeaveCriticalSection(&::g_CritSectionProtectingWoemEnterDll);

	TRACE_EXIT("WoemEnterDll");

	return returnValue;
}

#define WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR(_msg) do{strncpy(WoemErrorString, _msg, PACKET_ERRSTR_SIZE - 1); TRACE_MESSAGE(_msg);}while(0)

////////////////////////////////////////////////////////////////////
// Function called when a process loads the dll.
// If this is the first time the dll is loaded, we start the driver
// service
//
// NOTE: this function has logging/tracing code, do NOT call it
// from within DllMain!
// NOTE: this internal function does not protect the access to 
// the global variables (in particular g_hGlobalMutex.
////////////////////////////////////////////////////////////////////
BOOL WoemEnterDllInternal(HINSTANCE DllHandle, char *WoemErrorString)
{
	SECURITY_DESCRIPTOR sd;
	SECURITY_ATTRIBUTES sa;
	LONG lold;
	char ObjName[MAX_OBJNAME_LEN];
	UINT i;
	OSVERSIONINFO osVer;
	CHAR osArchitecture[256];
	HKEY environmentKey = NULL;
	DWORD keyType;
	DWORD bufSize;
    HRESULT hr;
#ifdef SECURITY
	DWORD Result;
#endif 

	TRACE_ENTER("WoemEnterDllInternal");

	WoemErrorString[PACKET_ERRSTR_SIZE - 1] = '\0';


#ifdef SECURITY

	TRACE_MESSAGE("Process bound Woem, checking if the process is authorized...");
	if (!g_IsProcAuthorized)
	{
	//
		Result = WoemGetCurrentProcessAuthorization(WoemErrorString);
		
		if (Result == WOEM_PROC_AUTHORIZATION_FAILURE)
		{
			g_IsProcAuthorized = FALSE;
			//the error string has been already set by WoemGetCurrentProcessAuthorization
			TRACE_EXIT("WoemEnterDllInternal");
			return FALSE;
			
		}
		else
			if (Result == WOEM_PROC_AUTHORIZATION_OK)
			{
				g_IsProcAuthorized = TRUE;
			}
			else
			{
				g_IsProcAuthorized = FALSE;
				
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("This version of WinPcap OEM can be only run in conjunction with CACE Technologies Network Toolkit. This program is not recognized as part of The Network Toolkit, and therefore WinPcap OEM will not work.");

				MessageBox(NULL, "This version of WinPcap OEM can be only run in conjunction with CACE Technologies Network Toolkit.\nThis program is not recognized as part of The Network Toolkit, and therefore WinPcap OEM will not work.", "Error", MB_ICONERROR);
				TRACE_EXIT("WoemEnterDllInternal");
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
		WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("NULL DLL Handle");

		TRACE_EXIT("WoemEnterDllInternal");
		return FALSE;
	}
#endif

	if(g_hGlobalMutex)
	{
		//
		// This should never happen, but better to be sure...
		//
		WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Double initialization");
		TRACE_EXIT("WoemEnterDllInternal");
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
		if (GetLastError() == ERROR_ACCESS_DENIED) 
		{
			WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Not enough priviles to create the global mutex.");
		}
		else
		{
			WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to create the global mutex.");
		}

		TRACE_EXIT("WoemEnterDllInternal");
		return FALSE;
	}

	//
	// Try to acquire the ownership of the mutex
	//
	DWORD result = WaitForSingleObject(g_hGlobalMutex, 10000);
	switch(result) 
	{
	case WAIT_FAILED:
		
		CloseHandle(g_hGlobalMutex);
		g_hGlobalMutex = NULL;

		WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Error trying to acquire the global mutex.");

		TRACE_EXIT("WoemEnterDllInternal");
		return FALSE;
		
	case WAIT_TIMEOUT:
		
		CloseHandle(g_hGlobalMutex);
		WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Timeout on the global mutex");

		TRACE_EXIT("WoemEnterDllInternal");
		return FALSE;
		
	case WAIT_OBJECT_0:
	case WAIT_ABANDONED:				
		break;
		
	}
	
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
		if (GetLastError() == ERROR_ACCESS_DENIED) 
		{
			WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Not enough priviles to create the global semaphore.");
		}
		else
		{
			WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to create the global semaphore.");
		}

		ReleaseMutex(g_hGlobalMutex);
		
		if(g_hGlobalMutex!=0)
		{
			CloseHandle(g_hGlobalMutex);
			g_hGlobalMutex = NULL;
			
		}
	
		TRACE_EXIT("WoemEnterDllInternal");
		return FALSE;
	}
	
	//
	// Check the value of the semaphore
	//
	if(ReleaseSemaphore(g_hGlobalSemaphore, 1, &lold)==FALSE)
	{
		WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to release the semaphore");
		
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
		
		TRACE_EXIT("WoemEnterDllInternal");
		return FALSE;
	}
	
	if (lold == 0)
	{
		//
		// IF WE ARE HERE, THE DLL IS BEING LOADED FOR THE FIRST TIME.
		//
		
		//
		// Get the OS we're running on
		//
		ZeroMemory(&osVer, sizeof(OSVERSIONINFO));
		osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		
		if (GetVersionEx(&osVer) == 0) 
		{
			WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to determine OS version");
	
			ReleaseMutex(g_hGlobalMutex);
			
			if(g_hGlobalMutex != 0)
			{
				CloseHandle(g_hGlobalMutex);
				g_hGlobalMutex = NULL;

			}
			if (g_hGlobalSemaphore!=0)
			{
				CloseHandle(g_hGlobalSemaphore);
				g_hGlobalSemaphore = NULL;
			}
			
			TRACE_EXIT("WoemEnterDllInternal");
			return FALSE;
		}
		
		//
		// Get the OS architecture
		//
		bufSize = sizeof(osArchitecture);
		if (RegOpenKey(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", &environmentKey) != ERROR_SUCCESS
			|| RegQueryValueEx(environmentKey, "PROCESSOR_ARCHITECTURE",	NULL,&keyType,(LPBYTE)&osArchitecture,&bufSize) != ERROR_SUCCESS
			|| keyType != REG_SZ)
		{
			WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to determine OS architecture");
	
			if (environmentKey != NULL)
				RegCloseKey(environmentKey);

			ReleaseMutex(g_hGlobalMutex);
			
			if(g_hGlobalMutex != 0)
			{
				CloseHandle(g_hGlobalMutex);
				g_hGlobalMutex = NULL;

			}
			if (g_hGlobalSemaphore!=0)
			{
				CloseHandle(g_hGlobalSemaphore);
				g_hGlobalSemaphore = NULL;
			}
			
			TRACE_EXIT("WoemEnterDllInternal");
			return FALSE;
		}

		//
		// check that we are running on x86 (the only architecture that we support at the moment)
		//
		if (stricmp("x86", osArchitecture) != 0)
		{
			WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unsupported Operating System architecture (only x86 is supported)");
	
			ReleaseMutex(g_hGlobalMutex);
			
			if(g_hGlobalMutex != 0)
			{
				CloseHandle(g_hGlobalMutex);
				g_hGlobalMutex = NULL;

			}
			if (g_hGlobalSemaphore!=0)
			{
				CloseHandle(g_hGlobalSemaphore);
				g_hGlobalSemaphore = NULL;
			}
			
			TRACE_EXIT("WoemEnterDllInternal");
			return FALSE;
		}

//  
//	Old registry based WinPcap names
//
//		//
//		// Create the WinPcap global registry key
//		//
//		if(!WoemCreateNameRegistryEntries())
//		{
//			WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to create registry entries. Administrator provileges are required for this operation");
//
//			ReleaseMutex(g_hGlobalMutex);
//			
//			if(g_hGlobalMutex!=0)
//			{
//				CloseHandle(g_hGlobalMutex);
//				g_hGlobalMutex = NULL;
//
//			}
//			if (g_hGlobalSemaphore!=0)
//			{
//				CloseHandle(g_hGlobalSemaphore);
//				g_hGlobalSemaphore = NULL;
//			}
//			
//			TRACE_EXIT("WoemEnterDllInternal");
//			return FALSE;
//		}

		if(!WoemCreateBinaryNames())
		{
			WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to create binary names");
	
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
			
			TRACE_EXIT("WoemEnterDllInternal");
			return FALSE;
		}
		
		if(check_if_service_is_running(NPF_DRIVER_NAME) == 0)
		{
			//
			// If we are here and the service is running, it's almost surely the result 
			// of some mess. We try to cleanup.
			//
			if (delete_service(NPF_DRIVER_NAME) == -1)
			{
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Error deleting an existing copy of the NPF service");

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
				
				TRACE_EXIT("WoemEnterDllInternal");
				return FALSE;
			}
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
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to copy the WinPcap OEM files. Administrative privileges are required for this operation.");

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
				
				TRACE_EXIT("WoemEnterDllInternal");
				return FALSE;
			}
			
			//
			// Extract the driver to disk
			//
			if(!WoemSaveResourceToDisk(g_DllHandle, IDP_DRINT, g_DriverFullPath))
			{
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to copy the WinPcap OEM files. Administrative privileges are required for this operation.");

				_unlink(g_DllFullPath);
				
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
				
				TRACE_EXIT("WoemEnterDllInternal");
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
					TRACE_MESSAGE("Warning: unable to load the netmon driver, ndiswan captures will not be available");
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
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to copy the WinPcap OEM files. Administrative privileges are required for this operation.");

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
				
				TRACE_EXIT("WoemEnterDllInternal");
				return FALSE;
			}
			
			//
			// Extract the driver to disk
			//
			if(!WoemSaveResourceToDisk(g_DllHandle, IDP_DRI2K, g_DriverFullPath))
			{
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to copy the WinPcap OEM files. Administrative privileges are required for this operation.");

				_unlink(g_DllFullPath);
				
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

				TRACE_EXIT("WoemEnterDllInternal");
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
			if(!WoemSaveResourceToDisk(g_DllHandle, IDP_DLLNT, g_DllFullPath))
			{
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to copy the WinPcap OEM files. Administrative privileges are required for this operation.");

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
				
				TRACE_EXIT("WoemEnterDllInternal");
				return FALSE;
			}
			
			//
			// Extract the driver to disk
			//
			if(!WoemSaveResourceToDisk(g_DllHandle, IDP_DRI2K, g_DriverFullPath))
			{
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to copy the WinPcap OEM files. Administrative privileges are required for this operation.");

				_unlink(g_DllFullPath);

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
				
				TRACE_EXIT("WoemEnterDllInternal");
				return FALSE;
			}
			
		}

		//
		// Create the driver service
		//		
		if(create_driver_service(NPF_DRIVER_NAME, 
			NPF_SERVICE_DESC, 
			NPF_DRIVER_COMPLETE_PATH) == -1)
		{
			WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("unable to create the packet driver service");

			_unlink(g_DllFullPath);
			_unlink(g_DriverFullPath);
			
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
			
			TRACE_EXIT("WoemEnterDllInternal");
			return FALSE;
		}

		//
		// Load the driver
		//
		if(start_service(NPF_DRIVER_NAME) == -1)
		{
			WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("unable to start the packet driver service");

			delete_service(NPF_DRIVER_NAME);
			_unlink(g_DllFullPath);
			_unlink(g_DriverFullPath);
			
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
			
			TRACE_EXIT("WoemEnterDllInternal");
			return FALSE;
		}

		//
		// Wait until the service is running
		//
		i = 0;
		while(TRUE)
		{
			if(check_if_service_is_running(NPF_DRIVER_NAME) == 0)
			{
				break;
			}
			
			i++;
			if(i == 300)
			{
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("timeout while starting the packet driver");
	
				delete_service(NPF_DRIVER_NAME);
				_unlink(g_DllFullPath);
				_unlink(g_DriverFullPath);
				
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
				
				TRACE_EXIT("WoemEnterDllInternal");
				return FALSE;
				
			}

			Sleep(100);
		}
		
		//
		// We've loaded the driver, we can delete its binary
		//
		_unlink(g_DriverFullPath);
	}
	else
	{
		if(!WoemCreateBinaryNames())
		{			
			delete_service(NPF_DRIVER_NAME);
			_unlink(g_DllFullPath);
			_unlink(g_DriverFullPath);
			
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
			
			TRACE_EXIT("WoemEnterDllInternal");
			return FALSE;
		}
	}
		
	//
	// Load packet.dll and intercept all the calls
	//
	if(!LoadPacketDll(g_DllFullPath, WoemErrorString))
	{
		delete_service(NPF_DRIVER_NAME);
		_unlink(g_DllFullPath);
		_unlink(g_DriverFullPath);
		
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
		
		TRACE_EXIT("WoemEnterDllInternal");
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

	TRACE_EXIT("WoemEnterDllInternal");
	return TRUE;
}

////////////////////////////////////////////////////////////////////
// Function called when a process loads the dll.
//
// NOTE: we cannot use tracing in this function, since it's called
// in the context of DllMain (MSDN clearly states that only the 
// functions exported by kernel32.dll can be called within the 
// DllMain).
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
			
			if(g_hGlobalMutex != 0)
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

		delete_service(NPF_DRIVER_NAME);
//  
//	Old registry based WinPcap names
//
//		WoemDeleteNameRegistryEntries();

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

#if 0
//  
//	Old registry based WinPcap names
//

////////////////////////////////////////////////////////////////////
// Create the temporary registry key with the name that all the  
// WinPcap components will use
////////////////////////////////////////////////////////////////////
BOOL WoemCreateNameRegistryEntries()
{
	HKEY WinpcapKey;
	OSVERSIONINFO osVer;

	ZeroMemory(&osVer,sizeof(OSVERSIONINFO));
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (GetVersionEx(&osVer)==0) 
	{
		TRACE_MESSAGE("Impossible to retrieve the OS version Info");
		return FALSE;
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
	// Add subkeys with the strings
	//

	//
	// NpfDriverName (e.g. "NPF")  -- HHH
	//
	if(RegSetValueEx(WinpcapKey, 
		NPF_DRIVER_NAME_REG_KEY, 
		0, 
		REG_SZ, 
		(LPBYTE)NPF_DRIVER_NAME,
		sizeof(NPF_DRIVER_NAME)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}
	
	//
	// NpfServiceDescription (e.g. "WinPcap Packet Driver ( NPF )") -- FFF
	//
	if(RegSetValueEx(WinpcapKey, 
		NPF_SERVICE_DESC_REG_KEY, 
		0,
		REG_SZ,
		(LPBYTE)NPF_SERVICE_DESC,
		sizeof(NPF_SERVICE_DESC)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	//
	//	NpfDeviceNamesPrefix (e.g. "NPF_") -- AAA
	//
	if(RegSetValueEx(WinpcapKey, 
		NPF_DEVICES_PREFIX_REG_KEY, 
		0,
		REG_SZ,
		(LPBYTE)NPF_DEVICE_NAMES_PREFIX,
		sizeof(NPF_DEVICE_NAMES_PREFIX)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

#if 0
	// npf_user_events_names

	memset(KeyValBuf, sizeof(KeyValBuf), 0);
	memset(KeyValBufW, sizeof(KeyValBufW), 0);

	_snwprintf(KeyValBufW, 
		sizeof(KeyValBufW) / sizeof(WCHAR) - 1, 
		L"%ws0000000000",
		g_NpfDriverNameIdW);

	if(RegSetValueEx(WinpcapKey, 
		NPF_USER_EVENTS_NAMES_REG_KEY, 
		0,
		REG_SZ,
		(LPBYTE)KeyValBufW,
		(wcslen(KeyValBufW) + 1) * sizeof(WCHAR)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}
#endif

	//
	// NpfEventsNames (e.g. "NPF") -- BBB
	//
	if(RegSetValueEx(WinpcapKey, 
		NPF_EVENTS_NAMES_REG_KEY, 
		0,
		REG_SZ,
		(LPBYTE)NPF_EVENTS_NAMES,
		sizeof(NPF_EVENTS_NAMES)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	//
	// NdiswanAdapterName (e.g. "\\Device\\NPF_GenericDialupAdapter") -- CCC
	//
	if(RegSetValueEx(WinpcapKey, 
		NPF_FAKE_NDISWAN_ADAPTER_NAME_REG_KEY, 
		0,
		REG_SZ,
		(LPBYTE)FAKE_NDISWAN_ADAPTER_NAME,
		sizeof(FAKE_NDISWAN_ADAPTER_NAME)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	//
	// NdiswanAdapterDescription (e.g. "Adapter for generic dialup and VPN capture") -- DDD
	//
	if(RegSetValueEx(WinpcapKey, 
		NPF_FAKE_NDISWAN_ADAPTER_DESC_REG_KEY, 
		0,
		REG_SZ,
		(LPBYTE)FAKE_NDISWAN_ADAPTER_DESCRIPTION,
		sizeof(FAKE_NDISWAN_ADAPTER_DESCRIPTION)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	//
	// NpfDriverCompletePath (e.g. "system32\\drivers\npf.sys") -- GG
	//
	if(RegSetValueEx(WinpcapKey,
		NPF_DRIVER_COMPLETE_PATH_REG_KEY, 
		0,
		REG_SZ,
		(LPBYTE)NPF_DRIVER_COMPLETE_PATH,
		sizeof(NPF_DRIVER_COMPLETE_PATH)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}
	
	//
	// NpfDriverCompleteDevicePrefix (e.g. "\\Device\\NPF_") -- III
	//
	if(RegSetValueEx(WinpcapKey,
		NPF_DRIVER_COMPLETE_DEVICE_PREFIX_REG_KEY,
		0,
		REG_SZ,
		(LPBYTE)NPF_DRIVER_COMPLETE_DEVICE_PREFIX,
		sizeof(NPF_DRIVER_COMPLETE_DEVICE_PREFIX)) != ERROR_SUCCESS)
	{
		RegCloseKey(WinpcapKey);
	
		RegDeleteKey(HKEY_LOCAL_MACHINE, WINPCAP_INSTANCE_KEY);
		return FALSE;
	}

	RegCloseKey(WinpcapKey);	

	return TRUE;
}
#endif

////////////////////////////////////////////////////////////////////
// Create the binaries names according to the name we have 
////////////////////////////////////////////////////////////////////
BOOL WoemCreateBinaryNames()
{
	//TODO we should try to put the files in three places:
	// - current directory
	// - system32
	// - temp directory
	UINT GsdRes;
	char WinDir[MAX_PATH + 16];
	char SysDir[MAX_PATH + 16];
	
	//
	// Get the location of the system folder to create the complete paths
	//
	GsdRes = GetSystemDirectory(SysDir, sizeof(SysDir));
	if(GsdRes == 0 || GsdRes == sizeof(SysDir))
	{
		return FALSE;
	}
	
	GsdRes = GetWindowsDirectory(WinDir, sizeof(WinDir));
	if(GsdRes == 0 || GsdRes == sizeof(WinDir))
	{
		return FALSE;
	}

	//
	// Created the strings that we'll use to build the registry keys values
	// and load the components
	//

	_snprintf(g_DllFullPath, 
		sizeof(g_DllFullPath) - 1, 
		"%s\\%swoem.tmp", 
		SysDir, 
		NPF_DRIVER_NAME);

	_snprintf(g_DriverFullPath, 
		sizeof(g_DriverFullPath) - 1, 
		"%s\\%s", 
		WinDir, 
		NPF_DRIVER_COMPLETE_PATH);

	return TRUE;
}

#if 0
//  
//	Old registry based WinPcap names
//

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

#endif
