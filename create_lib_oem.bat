@echo off

IF "%1"=="" (set WPDPACKDESTDIR=.\WpdPack\) ELSE (set WPDPACKDESTDIR=%1)

echo Creating \Lib folder
mkdir %WPDPACKDESTDIR% >nul 2>nul
mkdir %WPDPACKDESTDIR%\Lib >nul 2>nul

xcopy /v /Y .\wpcap\PRJ\Release_REMOTE\wpcap.lib			%WPDPACKDESTDIR%\Lib\
xcopy /v /Y .\OEM\WinPcapOEM\release\packet.lib				%WPDPACKDESTDIR%\Lib\

echo Folder \Lib created successfully

set WPDPACKDESTDIR=

