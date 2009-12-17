#include <stdio.h>
#include <io.h>
#include "Packet32.h"
#include "WpcapNames.h"
#include "WinpcapOem.h"
#include "resource.h"
#include "WoemDebug.h"

#ifdef TNT_BUILD
#include "Security.h"
#endif

HANDLE g_hGlobalMutex = NULL;
HANDLE g_hGlobalSemaphore = NULL;
char g_GlobalSemaphoreName[MAX_OBJNAME_LEN];
char g_DllFullPathNm[MAX_PATH + 16];
char g_DllFullPathNoNm[MAX_PATH + 16];
char *g_DllFullPath;
char g_DriverFullPath[MAX_PATH + 16];
HINSTANCE g_DllHandle = NULL;
char g_LastWoemError[PACKET_ERRSTR_SIZE];
volatile BOOL g_InitError = FALSE;
BOOL g_IsProcAuthorized = FALSE;
BOOL g_DisableNetMon = FALSE;

CRITICAL_SECTION g_CritSectionProtectingWoemEnterDll;

static VOID InitializePacketHandleForDllUnloadHandlesSweep();
static VOID SweepPacketHandles();

////////////////////////////////////////////////////////////////////
// DLL Entry point
////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HINSTANCE Dllh, DWORD Reason, LPVOID lpReserved)
{
	switch(Reason)
    {

	case DLL_PROCESS_ATTACH:
		
		g_DllHandle = Dllh;
		InitializeCriticalSection(&g_CritSectionProtectingWoemEnterDll);
		InitializePacketHandleForDllUnloadHandlesSweep();
		
#ifdef TNT_BUILD
		//
		// the version with security enabled doesn't need to be activated with PacketStartOem()
		//
		g_OemActive = TRUE;
#endif

		break;
		
	case DLL_PROCESS_DETACH:
		
		//
		// Before trying to tear down any driver, we need to make sure that all the open handles
		// to the NPF driver has been closed. The reason for this is quite complex. If there's even
		// single open handle to the NPF driver, the driver cannot be unloaded. When you try to stop
		// the driver service with the SCM (ControlService), the service will go into the SERVICE_STOP_PENDING
		// status until the last handle gets closed. But ControlService returns immediately, way before the service
		// has been stopped.
		// If after this you call DeleteService, the service is still running and is marked for deletion. When a service
		// is marked for deletion, you cannot reinstall it until it gets completely deleted. It's true that when the process
		// is killed by the OS, all the open handles are closed (hence the NPF driver is unloaded). But it takes a bit of time
		// between when the process is closed (hence the handles are closed) and when the driver is actually unloaded and the service
		// deleted. If in the meanwhile someone tries to restart the winpcap-pro based app, the attempt to reinstall the driver 
		// may fail because the driver is still in the "marked for deletion" status.
		// 
		// What we do here is closing the handles, and then the delete_service function makes sure that the service is stopped before deleting it, 
		// waiting for a certain amount of time. If by the timeout the service is not stopped, we do not delete the service.
		//
		SweepPacketHandles();
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

#define WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR(_msg) do{strncpy(WoemErrorString, _msg, PACKET_ERRSTR_SIZE - 1); TRACE_MESSAGE(_msg);}while(0)

BOOL WoemEnterDll(HINSTANCE DllHandle, char *WoemErrorString, ULONG flags)
{
	BOOL returnValue = TRUE;

	TRACE_ENTER("WoemEnterDll");
	
	if ((flags & (~PACKET_START_OEM_NO_NETMON)) != 0)
	{
		WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unsupported flags passed as parameter.");
		returnValue = FALSE;
	}
	else
	{
		EnterCriticalSection(&g_CritSectionProtectingWoemEnterDll);

		if (g_StillToInit)
		{
			if (flags & PACKET_START_OEM_NO_NETMON)
			{
				g_DisableNetMon = TRUE;
			}
			else
			{
				g_DisableNetMon = FALSE;
			}

			returnValue = WoemEnterDllInternal(DllHandle, WoemErrorString);
			if (returnValue == TRUE)
			{
				g_StillToInit = FALSE;
			}
		}
		else
		{
			if (flags & PACKET_START_OEM_NO_NETMON)
			{
				if (g_DisableNetMon == FALSE)
				{
					WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Already started WinPcap Professional with support for NetMon. Cannot restart it without NetMon support.");
					returnValue = FALSE;
				}
			}
			else
			{
				if (g_DisableNetMon == TRUE)
				{
					WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Already started WinPcap Professional without support for NetMon. Cannot restart it with NetMon support.");
					returnValue = TRUE;
				}
			}
		}
	}

	LeaveCriticalSection(&g_CritSectionProtectingWoemEnterDll);

	TRACE_EXIT("WoemEnterDll");

	return returnValue;
}


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
	BOOL npfDriverIsAlreadyRunning = FALSE;
#ifdef TNT_BUILD
	DWORD Result;
#endif 
	BOOL is64BitOs = FALSE;
	BOOL bLoadDriverResult;
	BOOL bUseNetMonPacket;

	TRACE_ENTER("WoemEnterDllInternal");

	WoemErrorString[PACKET_ERRSTR_SIZE - 1] = '\0';

#ifdef TNT_BUILD

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
				
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("This version of WinPcap Professional can be only run in conjunction with CACE Technologies Network Toolkit. This program is not recognized as part of The Network Toolkit, and therefore WinPcap Professional will not work.");

				MessageBox(NULL, "This version of WinPcap Professional can be only run in conjunction with CACE Technologies Network Toolkit.\nThis program is not recognized as part of The Network Toolkit, and therefore WinPcap Professional will not work.", "Error", MB_ICONERROR);
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

	//
	// Get the OS we're running on
	//
	ZeroMemory(&osVer, sizeof(OSVERSIONINFO));
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		
	if (GetVersionEx(&osVer) == 0) 
	{
		WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to determine OS version");
	
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

		TRACE_EXIT("WoemEnterDllInternal");
		return FALSE;
	}

	if (_stricmp("x86", osArchitecture) == 0)
	{
		is64BitOs = FALSE;
	}
	else 
	if (_stricmp("AMD64", osArchitecture) == 0)
	{
		is64BitOs = TRUE;
	}
	else
	{
		WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unsupported Operating System architecture (only x86 and AMD64 are supported)");
	
		TRACE_EXIT("WoemEnterDllInternal");
		return FALSE;
	}

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

		if (g_DisableNetMon == TRUE)
		{
			bUseNetMonPacket = FALSE;
		}
		else
		{
			if(osVer.dwMajorVersion == 5)
			{
				//
				// Windows 2000, XP, 2003
				//
				if (is64BitOs)
				{
					bUseNetMonPacket = FALSE;
				}
				else
				{
					bUseNetMonPacket = TRUE;
				}
			}
			else
			{
				//
				// all the other OSes, including NT4 and Vista
				//
				bUseNetMonPacket = FALSE;
			}

		}

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

		if (bUseNetMonPacket)
		{
			g_DllFullPath = g_DllFullPathNm;
		}
		else
		{
			g_DllFullPath = g_DllFullPathNoNm;
		}
		
		if(check_if_service_is_running(NPF_DRIVER_NAME) == 0)
		{
			//
			// if the service is running, just skip restarting it
			//
			TRACE_MESSAGE("The NPF driver (service " NPF_DRIVER_NAME ") is still running, do not reinstall it");
			npfDriverIsAlreadyRunning = TRUE;
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
			if(!WoemSaveResourceToDisk(g_DllHandle, IDP_DLLNT, g_DllFullPath, FALSE))
			{
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to copy the WinPcap Professional files. Administrative privileges are required for this operation.");

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
			if(!WoemSaveResourceToDisk(g_DllHandle, IDP_DRINT, g_DriverFullPath, FALSE))
			{
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to copy the WinPcap Professional files. Administrative privileges are required for this operation.");

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
			BOOL bExtractDllResult;

			//
			// Windows 2000, XP, 2003
			//
			
			if (g_DisableNetMon == FALSE)
			{
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
			}

			//
			// Extract packet.dll to disk, both ones
			//
			bExtractDllResult = WoemSaveResourceToDisk(g_DllHandle, IDP_DLL2K, g_DllFullPathNm, FALSE);
			bExtractDllResult &=WoemSaveResourceToDisk(g_DllHandle, IDP_DLLVISTA, g_DllFullPathNoNm, FALSE);

			if (!bExtractDllResult)
			{
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to copy the WinPcap Professional files. Administrative privileges are required for this operation.");

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
			if (is64BitOs)
			{
				bLoadDriverResult = WoemSaveResourceToDisk(g_DllHandle, IDP_DRIx64, g_DriverFullPath, TRUE);
			}
			else
			{
				bLoadDriverResult = WoemSaveResourceToDisk(g_DllHandle, IDP_DRI2K, g_DriverFullPath, FALSE);
			}

			if(!bLoadDriverResult)
			{
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to copy the WinPcap Professional files. Administrative privileges are required for this operation.");

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
			// Windows Vista or Win7 (6.1)
			//
			
			//
			// note: this will not work on vista 64 bit!!
			//

			//
			// Extract packet.dll to disk
			//
			if(!WoemSaveResourceToDisk(g_DllHandle, IDP_DLLVISTA, g_DllFullPath, FALSE))
			{
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to copy the WinPcap Professional files. Administrative privileges are required for this operation.");

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
			//
			// Extract the driver to disk
			//
			if (is64BitOs)
			{
				bLoadDriverResult = WoemSaveResourceToDisk(g_DllHandle, IDP_DRIx64, g_DriverFullPath, TRUE);
			}
			else
			{
				bLoadDriverResult = WoemSaveResourceToDisk(g_DllHandle, IDP_DRI2K, g_DriverFullPath, FALSE);
			}
			if(!bLoadDriverResult)
			{
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("Unable to copy the WinPcap Professional files. Administrative privileges are required for this operation.");

				_unlink(g_DllFullPathNm);
				_unlink(g_DllFullPathNoNm);

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

		if (!npfDriverIsAlreadyRunning)
		{

			//
			// Create the driver service
			//		
			if(create_driver_service(NPF_DRIVER_NAME, 
				NPF_SERVICE_DESC, 
				NPF_DRIVER_COMPLETE_PATH) == -1)
			{
				WOEM_ENTER_DLL_TRACE_AND_COPY_ERROR("unable to create the packet driver service");

				_unlink(g_DllFullPathNm);
				_unlink(g_DllFullPathNoNm);
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
				_unlink(g_DllFullPathNm);
				_unlink(g_DllFullPathNoNm);
				WoemDeleteDriverBinary(g_DriverFullPath, is64BitOs);

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

					WoemDeleteDriverBinary(g_DriverFullPath, is64BitOs);

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

		}
		//
		// We've loaded the driver, we can delete its binary
		//
//
// MCAFEE
//
//		WoemDeleteDriverBinary(g_DriverFullPath, is64BitOs);
	}
	else
	{
		if (g_DisableNetMon == TRUE)
		{
			bUseNetMonPacket = FALSE;
		}
		else
		{
			if(osVer.dwMajorVersion == 5)
			{
				//
				// Windows 2000, XP, 2003
				//
				if (is64BitOs)
				{
					bUseNetMonPacket = FALSE;
				}
				else
				{
					bUseNetMonPacket = TRUE;
				}
			}
			else
			{
				//
				// all the other OSes, including NT4 and Vista
				//
				bUseNetMonPacket = FALSE;
			}

		}

		if(!WoemCreateBinaryNames())
		{			
			delete_service(NPF_DRIVER_NAME);
			_unlink(g_DllFullPathNm);
			_unlink(g_DllFullPathNoNm);

			WoemDeleteDriverBinary(g_DriverFullPath, is64BitOs);
			
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

		if (bUseNetMonPacket)
		{
			g_DllFullPath = g_DllFullPathNm;
		}
		else
		{
			g_DllFullPath = g_DllFullPathNoNm;
		}
	}
		
	//
	// Load packet.dll and intercept all the calls
	//
	if(!LoadPacketDll(g_DllFullPath, WoemErrorString))
	{
		delete_service(NPF_DRIVER_NAME);
		_unlink(g_DllFullPathNm);
		_unlink(g_DllFullPathNoNm);
		WoemDeleteDriverBinary(g_DriverFullPath, is64BitOs);
		
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
	DeleteDll(g_DllFullPathNoNm);	
	DeleteDll(g_DllFullPathNm);	
	
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
///
/// MCAFEE
///
	BOOLEAN is64BitOs = FALSE;
	CHAR osArchitecture[256];
	HKEY environmentKey = NULL;
	DWORD keyType;
	DWORD bufSize;

	bufSize = sizeof(osArchitecture);
	if (RegOpenKey(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", &environmentKey) != ERROR_SUCCESS
		|| RegQueryValueEx(environmentKey, "PROCESSOR_ARCHITECTURE",	NULL,&keyType,(LPBYTE)&osArchitecture,&bufSize) != ERROR_SUCCESS
		|| keyType != REG_SZ)
	{
		//
		// we ignore the error and assume it's x86
		//
	}
	else
	if (_stricmp("AMD64", osArchitecture) == 0)
	{
		is64BitOs = TRUE;
		RegCloseKey(environmentKey);
	}

	if (environmentKey != NULL)
	{
		RegCloseKey(environmentKey);
	}

///
/// MCAFEE
///

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

///
/// MCAFEE
///
		WoemDeleteDriverBinary(g_DriverFullPath, is64BitOs);
///
/// MCAFEE
///

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
	
//	GsdRes = GetSystemWindowsDirectory(WinDir, sizeof(WinDir));
//	if(GsdRes == 0 || GsdRes == sizeof(WinDir))
//	{
//		return FALSE;
//	}

	//
	// NOTE: this is a patch to a problem with GetWindowsDirectory: as explained in the MSDN
	// docs, this API does NOT always return the windows folder like "c:\windows". On TS systems 
	// in some particular conditions (e.g. we saw it on a win2003-x64 machine) it returns a private
	// user folder (c:\documents and settinfs\user...). The right API is GetSystemWindowsDirectoryr, which
	// is not available on NT4. The MSDN docs suggests for NT4 to use GetSystemDirectory and trim system32 at
	// the end. We use this trick on every os (even if it's sufggested for NT4 only) beacuse in any case we have
	// hardcoded the system32 part in our NPF_ strings in wpcap_names.h (and we just write "system32\drivers\drivername.sys" 
	// for the path of the driver in the registry, not the full path).
	// GV 20060817
	//

	CopyMemory(WinDir, SysDir, sizeof(WinDir));

	//
	// trim the "system32" part
	//
	if (strlen(WinDir) < strlen("\\system32"))
	{
		return FALSE;
	}
	
	char *SupposedSystem32String;

	SupposedSystem32String = WinDir + strlen(WinDir) - strlen("\\system32");

	if (_stricmp(SupposedSystem32String, "\\system32") != 0)
	{
		return FALSE;
	}

	//
	// found it, just terminate it there
	//
	SupposedSystem32String[0] = '\0';


	//
	// Created the strings that we'll use to build the registry keys values
	// and load the components
	//

	_snprintf(g_DllFullPathNm, 
		sizeof(g_DllFullPathNm) - 1, 
		"%s\\%swoem_nm.tmp", 
		SysDir, 
		NPF_DRIVER_NAME);

	_snprintf(g_DllFullPathNoNm, 
		sizeof(g_DllFullPathNoNm) - 1, 
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

//
// note: this function has actually nothing to
BOOL WoemDeleteDriverBinary(char* FileName, BOOL bDisableFsRedirector)
{
	PVOID OldFsRedirectorValue;
	HMODULE hKernel32Dll;
	Wow64DisableWow64FsRedirectionHandler DisableFsRedirector;
	Wow64RevertWow64FsRedirectionHandler RevertFsRedirector;

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

	_unlink(FileName);

	if (bDisableFsRedirector)
	{

		if (RevertFsRedirector(OldFsRedirectorValue) == FALSE)
		{
			//
			// trace me
			//
			return FALSE;
		}

		FreeLibrary(hKernel32Dll);
	}

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

CRITICAL_SECTION g_PacketHandleSweepCriticalSection;
typedef struct _HANDLE_ITEM
{
	struct _HANDLE_ITEM	*Next;
	HANDLE				hFile;
}
	HANDLE_ITEM, *PHANDLE_ITEM;

PHANDLE_ITEM g_HandleList;

//
// Initialize the global list of open handles to the NPF driver
//
// Please see the long note in DllMain for the reason why we need
// this hack.
//
VOID InitializePacketHandleForDllUnloadHandlesSweep()
{
	TRACE_ENTER("InitializePacketHandleForDllUnloadHandlesSweep");
	g_HandleList = NULL;
	InitializeCriticalSection(&g_PacketHandleSweepCriticalSection);
	TRACE_EXIT("InitializePacketHandleForDllUnloadHandlesSweep");
}

//
// Register the open handle to the NPF driver in our global list
// The function returns TRUE if the registration was successful
// (or the ADAPTER instance is not related to an NPF adapter)
//
// Please see the long note in DllMain for the reason why we need
// this hack.
//
BOOL RegisterPacketHandleForDllUnloadHandlesSweep(LPADAPTER pAdapter)
{
	BOOL result = TRUE;
	PHANDLE_ITEM pItem;

	TRACE_ENTER("RegisterPacketHandleForDllUnloadHandlesSweep");
	if (pAdapter->Flags == INFO_FLAG_NDIS_ADAPTER && pAdapter->hFile != NULL && pAdapter->hFile != INVALID_HANDLE_VALUE)
	{
		pItem = (PHANDLE_ITEM)GlobalAlloc(GPTR, sizeof(HANDLE_ITEM));
		if (pItem == NULL)
		{
			result = FALSE;
		}
		else
		{
			pItem->hFile = pAdapter->hFile;
			EnterCriticalSection(&g_PacketHandleSweepCriticalSection);
			pItem->Next = g_HandleList;
			g_HandleList = pItem;
			LeaveCriticalSection(&g_PacketHandleSweepCriticalSection);
		}
	}

	TRACE_EXIT("RegisterPacketHandleForDllUnloadHandlesSweep");
	return result;
}

//
// Deregister the open handle to the NPF driver from our global list
// Please note that this function does NOT close the handle, it
// just removes it from the global list
//
// Please see the long note in DllMain for the reason why we need
// this hack.
//
VOID DeregisterPacketHandleForDllUnloadHandlesSweep(LPADAPTER pAdapter)
{
	PHANDLE_ITEM pCursor, pPrev;
	TRACE_ENTER("DeregisterPacketHandleForDllUnloadHandlesSweep");

	if (pAdapter->Flags == INFO_FLAG_NDIS_ADAPTER && pAdapter->hFile != NULL && pAdapter->hFile != INVALID_HANDLE_VALUE)
	{
		pPrev = NULL;

		EnterCriticalSection(&g_PacketHandleSweepCriticalSection);

		pCursor = g_HandleList;
		while(pCursor != NULL)
		{
			if (pCursor->hFile == pAdapter->hFile)
			{
				if (pPrev != NULL)
				{
					pPrev->Next = pCursor->Next;
				}
				else
				{
					g_HandleList = pCursor->Next;
				}
				
				GlobalFree(pCursor);
				break;
			}

			pPrev = pCursor;
			pCursor = pCursor->Next;
		}
		LeaveCriticalSection(&g_PacketHandleSweepCriticalSection);
	}
	
	TRACE_EXIT("DeregisterPacketHandleForDllUnloadHandlesSweep");

}

//
// Close all the open handles to the NPF driver that have been registered
// with RegisterPacketHandleForDllUnloadHandlesSweep
//
// Please see the long note in DllMain for the reason why we need
// this hack.
//
VOID SweepPacketHandles()
{
	PHANDLE_ITEM pCursor, pNext;
	
	TRACE_ENTER("SweepPacketHandles");

	//
	// NOTE: there is a possibility of a deadlock here. 
	// if the application dies when we were calling a (De)RegisterHandle
	// and the criticalsection was held, we would deadlock here.
	// 
	// What we do here is not locking at all.
	// In any case, g_HandleList always contains valid elements (the Deregister removes the item 
	// after having updated the list)
	//
	//
	//EnterCriticalSection(&g_PacketHandleSweepCriticalSection);

	pCursor = g_HandleList;

	while(pCursor != NULL)
	{
		pNext = pCursor->Next;

		(VOID)CloseHandle(pCursor->hFile);
		GlobalFree(pCursor);
		pCursor = pNext;
	}

	//LeaveCriticalSection(&g_PacketHandleSweepCriticalSection);

	TRACE_EXIT("SweepPacketHandles");

}
