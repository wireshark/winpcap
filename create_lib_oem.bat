@echo off

IF "%1"=="" (set WPDPACKDESTDIR=.\WpdPack\) ELSE (set WPDPACKDESTDIR=%1)

echo Creating \Lib folder
mkdir %WPDPACKDESTDIR% >nul 2>nul
mkdir %WPDPACKDESTDIR%\Lib >nul 2>nul
mkdir %WPDPACKDESTDIR%\Lib\x64 >nul 2>nul

xcopy /v /Y ".\wpcap\PRJ\Release REMOTE\x86\wpcap.lib"			%WPDPACKDESTDIR%\Lib\  >nul
xcopy /v /Y ".\wpcap\PRJ\Release REMOTE\x64\wpcap.lib"			%WPDPACKDESTDIR%\Lib\x64\  >nul
xcopy /v /Y .\OEM\WinPcapOEM\release\x86\packet.lib				%WPDPACKDESTDIR%\Lib\ 	>nul
xcopy /v /Y .\OEM\WinPcapOEM\release\x64\packet.lib				%WPDPACKDESTDIR%\Lib\x64\ 	>nul

echo Folder \Lib created successfully

set WPDPACKDESTDIR=

