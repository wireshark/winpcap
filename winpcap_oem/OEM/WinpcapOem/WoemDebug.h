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

#ifndef __WOEM_DEBUG_H_
#define __WOEM_DEBUG_H_

#include <windows.h>
#include <stdio.h>

//#define ENABLE_TRACE

#pragma warning ( push )
#pragma warning( disable : 4995 ) // deprecated string functions. BE CAREFUL, it disabled all deprecated warnings

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
	int loops = 0;
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

#pragma warning ( pop )


#if defined (_DEBUG) || defined (ENABLE_TRACE)

#define TRACE_ENTER(_x) OutputDebugMessageNewLine("-->" _x)
#define TRACE_EXIT(_x) OutputDebugMessageNewLine("<--" _x)
#define TRACE_MESSAGE OutputDebugMessageNewLine

#else

#define TRACE_ENTER(_x) do{}while(0)
#define TRACE_EXIT(_x)  do{}while(0)

static __inline void TRACE_MESSAGE(char *_x, ...){}

#endif

#endif //__WOEM_DEBUG_H_
