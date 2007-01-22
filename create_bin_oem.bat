@echo off

IF "%1"=="" (set WPDPACKDESTDIR=.\WpdPack\) ELSE (set WPDPACKDESTDIR=%1)

echo Creating \bin folder
mkdir %WPDPACKDESTDIR% >nul 2>nul
mkdir %WPDPACKDESTDIR%\bin >nul 2>nul

if not exist .\oem\WinPcapOem\Binaries_SIGNED\wpcap.dll (
	echo Not using the signed version of the wpcap.dll
	xcopy /v /Y .\wpcap\PRJ\Release_REMOTE\wpcap.dll	%WPDPACKDESTDIR%\bin\ >nul		
) else (
	echo Using the signed version of wpcap.dll
	xcopy /v /y .\oem\WinPcaPOem\binaries_signed\wpcap.dll %WPDPACKDESTDIR%\bin\ >nul
)

if not exist .\oem\WinPcapOem\Binaries_signed\packet.dll (
	echo Not using the signed version of packet.dll
	xcopy /v /Y .\OEM\WinPcapOEM\release\packet.dll				%WPDPACKDESTDIR%\bin\ >nul
) else (
	echo Using the signed version of packet.dll
	xcopy /v /y .\oem\WinPcaPOem\binaries_signed\packet.dll %WPDPACKDESTDIR%\bin\ >nul
)

echo Folder \bin created successfully

set WPDPACKDESTDIR=

