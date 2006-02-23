@echo off

if "%1"=="" (set __DEST_FOLDER=.\WpdPack\ )  ELSE (set __DEST_FOLDER=%1)

echo ************************************************
echo  The developer's pack will be created in folder
echo     %__DEST_FOLDER%
echo ************************************************
pause

call create_include_oem.bat ..\winpcap\ %__DEST_FOLDER%
PAUSE
call create_lib_oem.bat %__DEST_FOLDER%
PAUSE
call create_bin_oem.bat %__DEST_FOLDER%
PAUSE
call create_examples_oem.bat %__DEST_FOLDER%
PAUSE
call create_docs_oem.bat %__DEST_FOLDER%
echo Copying the license agreement file
xcopy /v "OEM WinPcap License Agreement.pdf" %__DEST_FOLDER% 

set __DEST_FOLDER=
