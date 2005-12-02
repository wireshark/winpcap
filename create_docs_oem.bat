@echo off

IF "%1"=="" (set WPDPACKDESTDIR=.\WpdPack\) ELSE (set WPDPACKDESTDIR=%1)

echo Creating \docs folder
mkdir %WPDPACKDESTDIR% >nul 2>nul
mkdir %WPDPACKDESTDIR%\docs >nul 2>nul
mkdir %WPDPACKDESTDIR%\docs\html >nul 2>nul

pushd dox\prj\

@echo #### Deleting existing WinPcap documentation
del /q docs\*.*
@echo #### Creating new documentation
doxygen winpcap_oem_noc.dox
@echo #### Copying all gif files
copy ..\pics\*.gif docs\. /v /y /q
copy ..\*.gif docs\. /v /y /q

popd

xcopy /v /Y ..\winpcap\dox\WinPcap_docs.html	%WPDPACKDESTDIR%\docs\
xcopy /v /Y .\dox\prj\docs\*.*			%WPDPACKDESTDIR%\docs\html\
xcopy /v /Y .\dox\*.gif				%WPDPACKDESTDIR%\docs\html\
xcopy /v /Y ..\winpcap\dox\*.gif		%WPDPACKDESTDIR%\docs\html\
xcopy /v /Y ..\winpcap\dox\pics\*.gif		%WPDPACKDESTDIR%\docs\html\
xcopy /v /Y .\dox\pics\*.gif			%WPDPACKDESTDIR%\docs\html\
echo Folder \docs created successfully
set WPDPACKDESTDIR=
set WINPCAPSOURCEDIR=
