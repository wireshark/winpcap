# Microsoft Developer Studio Project File - Name="WinpcapOem" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=WinpcapOem - Win32 Debug_Security
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WinpcapOem.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinpcapOem.mak" CFG="WinpcapOem - Win32 Debug_Security"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinpcapOem - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WinpcapOem - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WinpcapOem - Win32 Debug_Security" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WinpcapOem - Win32 Release_Security" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WinpcapOem - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WinpcapOem_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\\" /I "..\..\..\winpcap\common" /I "..\..\..\common" /D "NDEBUG" /D "_MBCS" /D "_USRDLL" /D "WinpcapOem_EXPORTS" /D "WIN32" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 npptools.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"Release/Packet.dll" /libpath:"..\..\WinpcapOem\Release\\"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=postbuild.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "WinpcapOem - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WinpcapOem_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\..\..\winpcap\common" /I "..\..\..\common" /D "_DEBUG" /D "_MBCS" /D "_USRDLL" /D "WinpcapOem_EXPORTS" /D "WIN32" /D "_WINDOWS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib npptools.lib /nologo /dll /debug /machine:I386 /out:"Debug/Packet.dll" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copying the OEM packet.lib in the winpcap folder
PostBuild_Cmds=postbuild.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "WinpcapOem - Win32 Debug_Security"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_Security"
# PROP BASE Intermediate_Dir "Debug_Security"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Security"
# PROP Intermediate_Dir "Debug_Security"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /D "_DEBUG" /D "_MBCS" /D "_USRDLL" /D "WinpcapOem_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "SECURITY" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\..\..\winpcap\common" /I "..\..\..\common" /D "_DEBUG" /D "_MBCS" /D "_USRDLL" /D "WinpcapOem_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "SECURITY" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib npptools.lib /nologo /dll /debug /machine:I386 /out:"Debug/Packet.dll" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib npptools.lib /nologo /dll /debug /machine:I386 /out:"Debug_Security/Packet.dll" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=postbuild.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "WinpcapOem - Win32 Release_Security"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_Security"
# PROP BASE Intermediate_Dir "Release_Security"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Security"
# PROP Intermediate_Dir "Release_Security"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I ".\\" /D "NDEBUG" /D "_MBCS" /D "_USRDLL" /D "WinpcapOem_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "SECURITY" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\\" /I "..\..\..\winpcap\common" /I "..\..\..\common" /D "NDEBUG" /D "_MBCS" /D "_USRDLL" /D "WinpcapOem_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "SECURITY" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 npptools.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"Release/Packet.dll" /libpath:"..\..\WinpcapOem\Release\\"
# ADD LINK32 npptools.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"Release_Security/Packet.dll" /libpath:"..\..\WinpcapOem\Release\\"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=postbuild.bat
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "WinpcapOem - Win32 Release"
# Name "WinpcapOem - Win32 Debug"
# Name "WinpcapOem - Win32 Debug_Security"
# Name "WinpcapOem - Win32 Release_Security"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Crypto.cpp
# End Source File
# Begin Source File

SOURCE=.\PacketWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\Resources.cpp
# End Source File
# Begin Source File

SOURCE=.\Security.cpp
# End Source File
# Begin Source File

SOURCE=.\SerivcesInstallers.cpp
# End Source File
# Begin Source File

SOURCE=.\WinPcapOem.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Crypto.h
# End Source File
# Begin Source File

SOURCE=.\idls.h
# End Source File
# Begin Source File

SOURCE=.\netcfgn.h
# End Source File
# Begin Source File

SOURCE=.\netcfgx.h
# End Source File
# Begin Source File

SOURCE=.\NetMonInstaller.h
# End Source File
# Begin Source File

SOURCE=.\pch.h
# End Source File
# Begin Source File

SOURCE=.\Security.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# Begin Source File

SOURCE=.\WinpcapOem.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\WpcapNames.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\binaries\npf2k.sys
# End Source File
# Begin Source File

SOURCE=.\binaries\Packet2k.dll
# End Source File
# Begin Source File

SOURCE=.\Packet2k.dll
# End Source File
# Begin Source File

SOURCE=.\WinpcapOem.def
# End Source File
# Begin Source File

SOURCE=.\WinPcapOem.rc
# End Source File
# End Target
# End Project
