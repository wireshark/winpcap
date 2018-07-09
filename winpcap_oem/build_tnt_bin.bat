@echo off

if "%1"=="" (set __DEST_FOLDER=.\TntBin)  ELSE (set __DEST_FOLDER=%1)

echo ******************************************************************
echo  The bin folder for The Network Toolkit will be created in folder
echo     %__DEST_FOLDER%
echo ******************************************************************
pause

echo Creating %__DEST_FOLDER% folder
mkdir %__DEST_FOLDER% >nul 2>nul

xcopy /v /Y .\wpcap\PRJ\Release_REMOTE\wpcap.dll		 %__DEST_FOLDER%\
xcopy /v /Y .\OEM\WinPcapOEM\release_TNT\packet.dll 		%__DEST_FOLDER%\

echo Folder %__DEST_FOLDER% created successfully

set __DEST_FOLDER=
