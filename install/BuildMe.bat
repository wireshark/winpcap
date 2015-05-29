@echo off

if  "%1" == "clean" (
  set __BUILD_TYPE=clean
) else (
  set __BUILD_TYPE=Build
)

rem Tell MSBuild to use environment variables.
rem set UseEnv=true

rem Issues with building both x86 and x64 from the same command prompt,
rem as only 1 architecture build type can be set
rem Maybe need a buildme_x86 and a buildme_x64, each one swetting their env.

msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Debug"      /p:Platform="Win32"
msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Debug No AirPcap"   /p:Platform="Win32"
msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Debug No NetMon"    /p:Platform="Win32"
msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Release"      /p:Platform="Win32"
msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Release No AirPcap"   /p:Platform="Win32"
msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Release No NetMon"    /p:Platform="Win32"
msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Release LOG_TO_FILE"    /p:Platform="Win32"
msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Release No NetMon LOG_TO_FILE" /p:Platform="Win32"

msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Debug"      /p:Platform="x64"
msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Debug No AirPcap"   /p:Platform="x64"
msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Debug No NetMon"    /p:Platform="x64"
msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Release"      /p:Platform="x64"
msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Release No AirPcap"   /p:Platform="x64"
msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Release No NetMon"    /p:Platform="x64"
msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Release LOG_TO_FILE"    /p:Platform="x64"
msbuild MakeAll.sln /t:%__BUILD_TYPE%  /p:Configuration="Release No NetMon LOG_TO_FILE" /p:Platform="x64"
