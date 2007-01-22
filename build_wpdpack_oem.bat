@echo off

if "%1"=="" (set WOEM_WPD_VANILLA_FOLDER=.\WpdPack\)  ELSE (set WOEM_WPD_VANILLA_FOLDER=%1)

echo ************************************************
echo  The developer's pack will be created in folder
echo     %WOEM_WPD_VANILLA_FOLDER%
echo ************************************************
rem pause

call create_include_oem.bat ..\winpcap\ %WOEM_WPD_VANILLA_FOLDER%
rem PAUSE
call create_lib_oem.bat %WOEM_WPD_VANILLA_FOLDER%
rem PAUSE
call create_bin_oem.bat %WOEM_WPD_VANILLA_FOLDER%
rem PAUSE
call create_examples_oem.bat %WOEM_WPD_VANILLA_FOLDER%
rem PAUSE
call create_docs_oem.bat %WOEM_WPD_VANILLA_FOLDER%
echo Copying the license agreement file
xcopy /v /Y "LicenseAgreement.pdf" %WOEM_WPD_VANILLA_FOLDER%. >nul
set WOEM_WPD_VANILLA_FOLDER=
