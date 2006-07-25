/*
 * Copyright (c) 2005 - 2006
 * CACE Technologies LLC, Davis (CA)
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
 * 3. Neither the name of the company (CACE Technologies LLC) nor the 
 * names of its contributors may be used to endorse or promote products 
 * derived from this software without specific prior written permission.
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

#ifndef __PACKET_DEBUG_393073863432093179878957
#define __PACKET_DEBUG_393073863432093179878957

#include <windows.h>
#include <stdio.h>

extern CHAR g_LogFileName[1024];

#if _DBG
#define ODS(_x) OutputDebugString(TEXT(_x))
#define ODSEx(_x, _y)
#else
#ifdef _DEBUG_TO_FILE

/*! 
  \brief Macro to print a debug string. The behavior differs depending on the debug level
*/

/*! 
  \brief Macro to print debug data with the printf convention. The behavior differs depending on 
  the debug level
*/
static __inline void OutputDebugToFile(char *_x, ...) 
{ 
	FILE *f;											
	SYSTEMTIME LocalTime;								
	va_list Marker;
	DWORD dwThreadId;
	int loops = 0;

	dwThreadId = GetCurrentThreadId();

	va_start( Marker, _x );     /* Initialize variable arguments. */
														
	GetLocalTime(&LocalTime);							
														
	do
	{
		
		f = fopen(g_LogFileName, "a");
		
		if (f != NULL)
			break;

		Sleep(0);
		loops++;

		if (loops > 10)
			return;
	}
	while(1);
	

	fprintf(f, "[%.08X] %.04u-%.02u-%.02u %.02u:%02u:%02u ",
			dwThreadId,
			LocalTime.wYear,							
			LocalTime.wMonth,							
			LocalTime.wDay,								
			LocalTime.wHour,							
			LocalTime.wMinute,							
			LocalTime.wSecond);										
	vfprintf(f, _x, Marker);
	
	fclose(f);											
}

static __inline void TRACE_PRINT_OS_INFO()
{
	HKEY	hKey;
	CHAR buffer[1024];
	DWORD size = sizeof(buffer);
	DWORD type;

	OutputDebugToFile("********************* OS info.********************* \n");
	buffer[size-1] = 0;
	size = sizeof(buffer);
	if(	RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueExA(hKey, "PROCESSOR_ARCHITECTURE", 0, &type, buffer, &size) == ERROR_SUCCESS && type == REG_SZ)
		{
			OutputDebugToFile("Architecture = %s\n", buffer);
		}
		else
		{
			OutputDebugToFile("Architecture = <UNKNOWN>\n");
		}
		
		RegCloseKey(hKey);
	}
	else
	{
		OutputDebugToFile("Architecture = <UNKNOWN>\n");
	}

	buffer[size-1] = 0;
	size = sizeof(buffer);

	if(	RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueExA(hKey, "CurrentVersion", 0, &type, buffer, &size) == ERROR_SUCCESS && type == REG_SZ)
		{
			OutputDebugToFile("Windows version = %s\n", buffer);
		}
		else
		{
			OutputDebugToFile("Windows version = <UNKNOWN>\n");
		}
		
		RegCloseKey(hKey);
	}
	else
	{
		OutputDebugToFile("Windows version = <UNKNOWN>\n");
	}

	buffer[size-1] = 0;
	size = sizeof(buffer);
	if(	RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueExA(hKey, "CurrentType", 0, &type, buffer, &size) == ERROR_SUCCESS && type == REG_SZ)
		{
			OutputDebugToFile("Windows CurrentType = %s\n", buffer);
		}
		else
		{
			OutputDebugToFile("Windows CurrentType = <UNKNOWN>\n");
		}
		
		RegCloseKey(hKey);
	}
	else
	{
		OutputDebugToFile("Windows CurrentType = <UNKNOWN>\n");
	}

	OutputDebugToFile("*************************************************** \n");


}

#define ODSEx OutputDebugToFile

__inline static  void ODS(char* _x) 
{										
	OutputDebugToFile(_x);
}

LONG PacketDumpRegistryKey(PCHAR KeyName, PCHAR FileName);
#else

static __inline void ODSEx(char *_x, ...){} 
static __inline void ODS(char *_x){} 
#define TRACE_PRINT_OS_INFO() 
//#define ODS(_x)		
//#define ODSEx(_x, _y)
#endif
#endif

#define TRACE_ENTER(_x) ODS("-->" _x "\n")
#define TRACE_EXIT(_x) ODS("<--" _x "\n")


#endif //__PACKET_DEBUG_393073863432093179878957
