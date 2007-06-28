@echo off

IF "%2"=="" (set WPDPACKDESTDIR=.\WpdPack\) ELSE (set WPDPACKDESTDIR=%2)

IF ""=="%1" (set WINPCAPSOURCEDIR=.\) ELSE (set WINPCAPSOURCEDIR=%1) 

echo Creating \docs folder
mkdir %WPDPACKDESTDIR% >nul 2>nul
mkdir %WPDPACKDESTDIR%\docs >nul 2>nul
mkdir %WPDPACKDESTDIR%\docs\html >nul 2>nul

pushd %CD%

chdir %WINPCAPSOURCEDIR%
call createdoc.bat

popd

xcopy /v /Y %WINPCAPSOURCEDIR%\dox\WinPcap_docs.html	%WPDPACKDESTDIR%\docs\ 		>nul
xcopy /v /Y %WINPCAPSOURCEDIR%\dox\prj\docs\*.*		%WPDPACKDESTDIR%\docs\html\	>nul
xcopy /v /Y %WINPCAPSOURCEDIR%\dox\*.gif		%WPDPACKDESTDIR%\docs\html\	>nul
xcopy /v /Y %WINPCAPSOURCEDIR%\dox\pics\*.gif		%WPDPACKDESTDIR%\docs\html\	>nul
echo Folder \docs created successfully
set WPDPACKDESTDIR=
set WINPCAPSOURCEDIR=
