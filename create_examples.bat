@echo off

IF "%2"=="" (set WPDPACKDESTDIR=.\WpdPack\) ELSE (set WPDPACKDESTDIR=%2)

IF ""=="%1" (set WINPCAPSOURCEDIR=.\) ELSE (set WINPCAPSOURCEDIR=%1) 

echo Creating \Examples folder
mkdir %WPDPACKDESTDIR% >nul 2>nul
mkdir %WPDPACKDESTDIR%\Examples >nul 2>nul

xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\Examples		%WPDPACKDESTDIR%\Examples\ 

echo Folder \Examples created successfully
set WPDPACKDESTDIR=
set WINPCAPSOURCEDIR=

echo ********************************************************************
echo *									*
echo * Now you can build the examples from the developers' pack folder! *
echo *									*
echo ********************************************************************
