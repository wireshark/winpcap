@echo off

IF "%1"=="" (set WPDPACKDESTDIR=.\WpdPack\) ELSE (set WPDPACKDESTDIR=%1)

IF NOT EXIST .\Examples (
	echo Copying the examples locally...
	call create_local_examples.bat %1 %2 %3
	)

echo Creating \docs folder
mkdir %WPDPACKDESTDIR% >nul 2>nul
mkdir %WPDPACKDESTDIR%\docs >nul 2>nul
mkdir %WPDPACKDESTDIR%\docs\html >nul 2>nul

pushd dox\prj\

@echo #### Deleting existing WinPcap documentation
del /q docs\*.*  >nul
@echo #### Creating new documentation
doxygen winpcap_oem_noc.dox > nul
@echo #### Copying all gif files
xcopy ..\pics\*.gif docs\.	/v /y /q > nul
xcopy ..\pics\*.jpg docs\. /v /y /q > nul

popd

xcopy /v /Y .\dox\WinPcapPro_docs.html		%WPDPACKDESTDIR%\docs\	 >nul
xcopy /v /Y .\dox\prj\docs\*.*			%WPDPACKDESTDIR%\docs\html\ >nul
xcopy /v /Y ..\winpcap\dox\*.gif		%WPDPACKDESTDIR%\docs\html\ >nul
xcopy /v /Y ..\winpcap\dox\pics\*.gif		%WPDPACKDESTDIR%\docs\html\ >nul
xcopy /v /Y .\dox\pics\*.gif			%WPDPACKDESTDIR%\docs\html\ >nul
xcopy /v /Y .\dox\pics\*.jpg			%WPDPACKDESTDIR%\docs\html\ >nul
echo Folder \docs created successfully
set WPDPACKDESTDIR=
set WINPCAPSOURCEDIR=
