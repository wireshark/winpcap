@echo off

IF NOT EXIST .\Examples (
	echo Copying the examples locally...
	call create_local_examples.bat
	)


IF "%1"=="" (set WPDPACKDESTDIR=.\WpdPack\) ELSE (set WPDPACKDESTDIR=%1)

set WINPCAPSOURCEDIR=.\

echo Creating \Examples folder
mkdir %WPDPACKDESTDIR% >nul 2>nul
mkdir %WPDPACKDESTDIR%\Examples-pcap >nul 2>nul
mkdir %WPDPACKDESTDIR%\Examples-remote >nul 2>nul

xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\Examples		%WPDPACKDESTDIR%\Examples-remote
 
rem *** Delete Netmeter since it's no more part of the Developer's pack *** 
rd /S /Q %WPDPACKDESTDIR%\Examples-remote\NetMeter\

rem *** Delete kdump since the feature is not available at the moment *** 
rd /S /Q %WPDPACKDESTDIR%\Examples-remote\kdump\

xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\Examples-pcap		%WPDPACKDESTDIR%\Examples-pcap

echo Folder \Examples created successfully
set WPDPACKDESTDIR=
set WINPCAPSOURCEDIR=

echo ********************************************************************
echo *									*
echo * Now you can build the examples from the developers' pack folder! *
echo *									*
echo ********************************************************************
