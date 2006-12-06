@echo off

set OEM_WPD_NPFIM_DEST_FOLDER=.\WpdPack_NpfIm\

if "%1"=="-CERTIFIED"	(
		echo Using NpfImLoader CERTIFIED DEVPACK
		set NPFIM_LOADER_DEVPACK=..\NpfIm\NpfImLoader_DevPack_CERTIFIED
		) else (
		echo use -CERTIFIED to use NpfImLoader CERTIFIED DEVPACK
		set NPFIM_LOADER_DEVPACK=..\NpfIm\NpfImLoader_DevPack
		)

set NPFIM_DEVPACK=..\NpfIm\NpfIm_DevPack

echo ************************************************
echo  The developer's pack will be created in folder
echo     %OEM_WPD_NPFIM_DEST_FOLDER%
echo ************************************************

call create_include_oem.bat ..\winpcap\ %OEM_WPD_NPFIM_DEST_FOLDER%
call create_lib_oem.bat %OEM_WPD_NPFIM_DEST_FOLDER%
call create_bin_oem.bat %OEM_WPD_NPFIM_DEST_FOLDER%
call create_examples_oem.bat %OEM_WPD_NPFIM_DEST_FOLDER%
call create_docs_oem.bat %OEM_WPD_NPFIM_DEST_FOLDER%

echo Copying the license agreement file
xcopy /v /Y "OEM WinPcap License Agreement.pdf" %OEM_WPD_NPFIM_DEST_FOLDER% >nul

rem now we copy the certified files from the NpfIm engine

echo Copying NpfIm specific files

rem NOTE: we do not copy all the lib/h files, only the ones really needed,
rem i.e. the ones for the loader
xcopy /v /Y  %NPFIM_LOADER_DEVPACK%\*.h*		%OEM_WPD_NPFIM_DEST_FOLDER%\include\ >nul
xcopy /v /Y  %NPFIM_LOADER_DEVPACK%\*.lib		%OEM_WPD_NPFIM_DEST_FOLDER%\lib\ >nul

rem NOTE: but we copy all the binaries...
xcopy /v /Y  %NPFIM_LOADER_DEVPACK%\*.dll		%OEM_WPD_NPFIM_DEST_FOLDER%\bin\ >nul
xcopy /v /Y  %NPFIM_DEVPACK%\*.dll			%OEM_WPD_NPFIM_DEST_FOLDER%\bin\ >nul

rem copy the available sample, IfListNpfIm. We assume that the sample is in ..\NpfIm\Sample

mkdir %OEM_WPD_NPFIM_DEST_FOLDER%\Examples-Remote\IfListNpfIm >nul 2>nul
xcopy /v /Y ..\NpfIm\Samples\IfListNpfIm\*.c* %OEM_WPD_NPFIM_DEST_FOLDER%\Examples-Remote\IfListNpfIm\ > nul
xcopy /v /Y ..\NpfIm\Samples\IfListNpfIm\*.h* %OEM_WPD_NPFIM_DEST_FOLDER%\Examples-Remote\IfListNpfIm\ > nul
xcopy /v /Y ..\NpfIm\Samples\IfListNpfIm\*.vcproj %OEM_WPD_NPFIM_DEST_FOLDER%\Examples-Remote\IfListNpfIm\ > nul
xcopy /v /Y ..\NpfIm\Samples\IfListNpfIm\*.sln %OEM_WPD_NPFIM_DEST_FOLDER%\Examples-Remote\IfListNpfIm\ > nul

rem copy the documentation for npfimLoader
mkdir %OEM_WPD_NPFIM_DEST_FOLDER%\docs\NpfImLoader 2>nul >nul
xcopy /s/e/v/Y ..\NpfIm\LowLevelDll\docs\NpfImLoader\*.* %OEM_WPD_NPFIM_DEST_FOLDER%\docs\NpfImLoader\ > nul
del /f/s/q %OEM_WPD_NPFIM_DEST_FOLDER%\docs\WinPcap >nul 2>nul
rd %OEM_WPD_NPFIM_DEST_FOLDER%\docs\WinPcap >nul 2>nul
ren %OEM_WPD_NPFIM_DEST_FOLDER%\docs\html WinPcap
copy /v ..\NpfIm\LowLevelDll\docs\OEM_WinPcap_docs.html %OEM_WPD_NPFIM_DEST_FOLDER%\docs\. > nul

set OEM_WPD_NPFIM_DEST_FOLDER=