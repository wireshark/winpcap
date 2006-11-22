@echo off


rem call build_wpdpack_oem.bat %1

if "%1"=="" (set __DEST_FOLDER=.\WpdPack\)  ELSE (set __DEST_FOLDER=%1)

rem now we copy the certified files from the NpfIm engine

echo Copying NpfIm specific files
pause

rem NOTE: we do not copy all the lib/h files, only the ones really needed,
rem i.e. the ones for the loader
xcopy /v /Y  ..\NpfIm_DevPack\NpfImLoader.h		%__DEST_FOLDER%\include\ >nul
xcopy /v /Y  ..\NpfIm_DevPack\NetCfgx.h		%__DEST_FOLDER%\include\ >nul
xcopy /v /Y  ..\NpfIm_DevPack\NpfImLoader.lib		%__DEST_FOLDER%\lib\ >nul

rem NOTE: but we copy all the binaries...
xcopy /v /Y  ..\NpfIm_DevPack\*.dll		%__DEST_FOLDER%\bin\ >nul

rem copy the available sample, IfListNpfIm. We assume that the sample is in ..\NpfIm\Sample

mkdir %__DEST_FOLDER%\Examples-Remote\IfListNpfIm >nul 2>nul
xcopy /v /Y ..\NpfIm\Samples\IfListNpfIm\*.c* %__DEST_FOLDER%\Examples-Remote\IfListNpfIm\ > nul
xcopy /v /Y ..\NpfIm\Samples\IfListNpfIm\*.h* %__DEST_FOLDER%\Examples-Remote\IfListNpfIm\ > nul
xcopy /v /Y ..\NpfIm\Samples\IfListNpfIm\*.vcproj %__DEST_FOLDER%\Examples-Remote\IfListNpfIm\ > nul
xcopy /v /Y ..\NpfIm\Samples\IfListNpfIm\*.sln %__DEST_FOLDER%\Examples-Remote\IfListNpfIm\ > nul

rem copy the documentation for npfimLoader
mkdir %__DEST_FOLDER%\docs\NpfImLoader 2>nul >nul
xcopy /s/e/v/Y ..\NpfIm_DevPack\docs\NpfImLoader %__DEST_FOLDER%\docs\NpfImLoader\ > nul

set __DEST_FOLDER=