@echo off

IF "%2"=="" (set WPDPACKDESTDIR=.\WpdPack\) ELSE (set WPDPACKDESTDIR=%2)

IF ""=="%1" (set WINPCAPSOURCEDIR=.\) ELSE (set WINPCAPSOURCEDIR=%1) 

echo Creating \Lib folder
mkdir %WPDPACKDESTDIR% >nul 2>nul
mkdir %WPDPACKDESTDIR%\Lib >nul 2>nul

xcopy /v /Y %WINPCAPSOURCEDIR%\wpcap\PRJ\Release_REMOTE\wpcap.lib			%WPDPACKDESTDIR%\Lib\ >nul
xcopy /v /Y %WINPCAPSOURCEDIR%\packetNtx\Dll\Project\Release\packet.lib		 	%WPDPACKDESTDIR%\Lib\ >nul
xcopy /v /Y %WINPCAPSOURCEDIR%\packetNtx\Dll\Project\libpacket.a			%WPDPACKDESTDIR%\Lib\	>nul
xcopy /v /Y %WINPCAPSOURCEDIR%\wpcap\LIB\libwpcap.a					%WPDPACKDESTDIR%\Lib\ >nul

echo Folder \Lib created successfully

set WPDPACKDESTDIR=
set WINPCAPSOURCEDIR=

