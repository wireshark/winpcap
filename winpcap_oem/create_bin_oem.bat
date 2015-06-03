@echo off

IF "%1"=="" (set WPDPACKDESTDIR=.\WpdPack\) ELSE (set WPDPACKDESTDIR=%1)

echo Creating \bin folder
mkdir %WPDPACKDESTDIR% >nul 2>nul
mkdir %WPDPACKDESTDIR%\bin >nul 2>nul
mkdir %WPDPACKDESTDIR%\bin\x64 >nul 2>nul
mkdir %WPDPACKDESTDIR%\bin\x86 >nul 2>nul

rem if not exist .\oem\WinPcapOem\Binaries_SIGNED\wpcap.dll (
rem	echo Not using the signed version of the wpcap.dll
rem	xcopy /v /Y .\wpcap\PRJ\Release_REMOTE\wpcap.dll	%WPDPACKDESTDIR%\bin\ >nul		
rem ) else (
rem	echo Using the signed version of wpcap.dll
rem	xcopy /v /y .\oem\WinPcaPOem\binaries_signed\wpcap.dll %WPDPACKDESTDIR%\bin\ >nul
rem )
rem 
rem if not exist .\oem\WinPcapOem\Binaries_signed\packet.dll (
rem	echo Not using the signed version of packet.dll
rem	xcopy /v /Y .\OEM\WinPcapOEM\release\packet.dll				%WPDPACKDESTDIR%\bin\ >nul
rem ) else (
rem	echo Using the signed version of packet.dll
rem	xcopy /v /y .\oem\WinPcaPOem\binaries_signed\packet.dll %WPDPACKDESTDIR%\bin\ >nul
rem )


xcopy /v /y ".\wpcap\PRJ\Release REMOTE\x86\wpcap.dll" %WPDPACKDESTDIR%\bin\x86\ >nul
xcopy /v /y ".\wpcap\PRJ\Release REMOTE\x64\wpcap.dll" %WPDPACKDESTDIR%\bin\x64\ >nul
xcopy /v /y ".\OEM\WinpcapOem\Release\x86\packet.dll"  %WPDPACKDESTDIR%\bin\x86\ >nul
xcopy /v /y ".\OEM\WinpcapOem\Release\x64\packet.dll"  %WPDPACKDESTDIR%\bin\x64\ >nul

echo Folder \bin created successfully

set WPDPACKDESTDIR=

