@echo off

IF "%1"=="" (set WPDPACKDESTDIR=.\WpdPack\) ELSE (set WPDPACKDESTDIR=%1)

echo Creating \bin folder
mkdir %WPDPACKDESTDIR% >nul 2>nul
mkdir %WPDPACKDESTDIR%\bin >nul 2>nul

xcopy /v /Y .\wpcap\PRJ\Release_REMOTE\wpcap.dll			%WPDPACKDESTDIR%\bin\ >nul
xcopy /v /Y .\OEM\WinPcapOEM\release\packet.dll				%WPDPACKDESTDIR%\bin\ >nul

echo Folder \bin created successfully

set WPDPACKDESTDIR=

