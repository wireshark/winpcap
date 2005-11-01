/*
 * Copyright (c) 2005
 * CACE Technologies LLC, Davis (CA)
 * All rights reserved.
 *
 */

#ifndef __WOEM_DEBUG_H_
#define __WOEM_TRACE_H_

#include <windows.h>
#include <stdio.h>


/*! 
  \brief Macro to print a debug string. The behavior differs depending on the debug level
*/

/*! 
  \brief Macro to print debug data with the printf convention. The behavior differs depending on 
  the debug level
*/
static __inline void CreateMessageStringV(char *_x, va_list Marker, BOOL newLine, char* outBuffer, size_t outBufferSize) 
{ 
	SYSTEMTIME LocalTime;								
	DWORD dwThreadId;
	size_t currentLen;

	DWORD dwLastError = GetLastError();

	dwThreadId = GetCurrentThreadId();

	GetLocalTime(&LocalTime);							
	
	outBuffer[outBufferSize - 1] = '\0';

	_snprintf(outBuffer, outBufferSize, "[%.08X] %.04u-%.02u-%.02u %.02u:%02u:%02u ",
			dwThreadId,
			LocalTime.wYear,							
			LocalTime.wMonth,							
			LocalTime.wDay,								
			LocalTime.wHour,							
			LocalTime.wMinute,							
			LocalTime.wSecond);										
	
	currentLen = strlen(outBuffer);
	
	::_vsnprintf(outBuffer + currentLen, outBufferSize - currentLen, _x, Marker);

	if (newLine)
	{
		currentLen = strlen(outBuffer);

		if (currentLen + 1 < outBufferSize)
		{
			outBuffer[currentLen] = '\n';
			outBuffer[currentLen + 1] = '\0';
		}
	}

	SetLastError(dwLastError);

}

static __inline void OutputDebugMessage(char *_x, ...)
{
#ifdef _DEBUG_TO_FILE
	FILE *f;
	int loops;
#endif


	va_list Marker;
	char message[1024];

	DWORD dwLastError = GetLastError();
	
	va_start(Marker, _x);

	CreateMessageStringV(_x, Marker, FALSE, message, sizeof(message));


#ifdef _DEBUG_TO_FILE
	do
	{
		f = fopen("woem_debug.txt", "a");
		if (f != NULL)
			break;

		Sleep(0);
		loops++;

		if (loops > 10)
			return;
	}
	while(1);

	fputs(message, f);

	fclose(f);
#else
	OutputDebugStringA(message);
#endif

	SetLastError(dwLastError);

}

static __inline void OutputDebugMessageNewLine(char *_x, ...)
{
#ifdef _DEBUG_TO_FILE
	FILE *f;
	int loops;
#endif

	va_list Marker;

	DWORD dwLastError = GetLastError();

	va_start(Marker, _x);
	char message[1024];

	CreateMessageStringV(_x, Marker, TRUE, message, sizeof(message));


#ifdef _DEBUG_TO_FILE
	do
	{
		f = fopen("woem_debug.txt", "a");
		if (f != NULL)
			break;

		Sleep(0);
		loops++;

		if (loops > 10)
			return;
	}
	while(1);

	fputs(message, f);

	fclose(f);
#else
	OutputDebugStringA(message);
#endif

	SetLastError(dwLastError);

}


static __inline void TRACE_PRINT_OS_INFO()
{
	HKEY	hKey;
	CHAR buffer[1024];
	DWORD size = sizeof(buffer);
	DWORD type;

	DWORD dwLastError = GetLastError();

	OutputDebugMessage("********************* OS info.********************* \n");
	buffer[size-1] = 0;
	size = sizeof(buffer);
	if(	RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueExA(hKey, "PROCESSOR_ARCHITECTURE", 0, &type, (LPBYTE)buffer, &size) == ERROR_SUCCESS && type == REG_SZ)
		{
			OutputDebugMessage("Architecture = %s\n", buffer);
		}
		else
		{
			OutputDebugMessage("Architecture = <UNKNOWN>\n");
		}
		
		RegCloseKey(hKey);
	}
	else
	{
		OutputDebugMessage("Architecture = <UNKNOWN>\n");
	}

	buffer[size-1] = 0;
	size = sizeof(buffer);

	if(	RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueExA(hKey, "CurrentVersion", 0, &type, (LPBYTE)buffer, &size) == ERROR_SUCCESS && type == REG_SZ)
		{
			OutputDebugMessage("Windows version = %s\n", buffer);
		}
		else
		{
			OutputDebugMessage("Windows version = <UNKNOWN>\n");
		}
		
		RegCloseKey(hKey);
	}
	else
	{
		OutputDebugMessage("Windows version = <UNKNOWN>\n");
	}

	buffer[size-1] = 0;
	size = sizeof(buffer);
	if(	RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueExA(hKey, "CurrentType", 0, &type, (LPBYTE)buffer, &size) == ERROR_SUCCESS && type == REG_SZ)
		{
			OutputDebugMessage("Windows CurrentType = %s\n", buffer);
		}
		else
		{
			OutputDebugMessage("Windows CurrentType = <UNKNOWN>\n");
		}
		
		RegCloseKey(hKey);
	}
	else
	{
		OutputDebugMessage("Windows CurrentType = <UNKNOWN>\n");
	}

	OutputDebugMessage("*************************************************** \n");

	SetLastError(dwLastError);

}

#ifdef _DEBUG

#define TRACE_ENTER(_x) OutputDebugMessageNewLine("-->" _x)
#define TRACE_EXIT(_x) OutputDebugMessageNewLine("<--" _x)
#define TRACE_MESSAGE OutputDebugMessageNewLine

#else

#define TRACE_ENTER(_x) do{}while(0)
#define TRACE_EXIT(_x)  do{}while(0)

static __inline void TRACE_MESSAGE(char *_x, ...){}

#endif

#endif //__WOEM_DEBUG_H_