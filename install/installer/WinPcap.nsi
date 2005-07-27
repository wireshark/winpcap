; Copyright (c) 2005
; CACE Technologies
; All rights reserved.
; 
; Redistribution and use in source and binary forms, with or without 
; modification, are permitted provided that the following conditions 
; are met:
; 
; 1. Redistributions of source code must retain the above copyright 
; notice, this list of conditions and the following disclaimer.
; 2. Redistributions in binary form must reproduce the above copyright 
; notice, this list of conditions and the following disclaimer in the 
; documentation and/or other materials provided with the distribution. 
; 3. Neither the name of the Politecnico di Torino nor the names of its 
; contributors may be used to endorse or promote products derived from 
; this software without specific prior written permission. 
; 
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
; "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
; LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
; A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
; OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
; SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
; LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
; DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
; THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
; OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; 
; This is the NSIS project for WinPcap installation. It will require NSIS
; from http://nsis.sourceforge.net/, plus three plugins:
;
;   - nsweb (http://nsis.sourceforge.net/wiki/NsWeb:_A_plugin_to_display_the_web_browser_control_in_a_custom_page)
;   - Internet (http://nsis.sourceforge.net/wiki/Internet_plugin)
;   - ExecDos (http://nsis.sourceforge.net/wiki/ExecDos)
;

;This includes the modern user interface
!include "MUI.nsh"

;--------------------------------
;Project definitions

  !define WINPCAP_PRJ_MAJOR "3"
  !define WINPCAP_PRJ_MINOR "1"
  !define WINPCAP_PRJ_REV "0"
  !define WINPCAP_PRJ_BUILD "26"
  !define WINPCAP_PROJ_VERSION_DOTTED "3.1.0.26"
  !define WINPCAP_LEGAL_COPYRIGHT "© 2005 CACE Technologies"
  !define WINPCAP_PRODUCT_NAME "WinPcap 3.1"
  !define WINPCAP_COMPANY_NAME "CACE Technologies"
  !define WINPCAP_FILE_NAME "WinPcap_${WINPCAP_PRJ_MAJOR}_${WINPCAP_PRJ_MINOR}.exe"
 
  ;Default installation folder
  InstallDir "$PROGRAMFILES\WinPcap"


;--------------------------------
;Includes and definitions


  ;the following two lines include the "Locate" function to find files
  !include "FileFunc.nsh"
  !insertmacro Locate

  !define VAR_0 0 ;$0
  !define VAR_1 1 ;$1
  !define VAR_2 2 ;$2
  !define VAR_3 3 ;$3
  !define VAR_4 4 ;$4
  !define VAR_5 5 ;$5
  !define VAR_6 6 ;$6
  !define VAR_7 7 ;$7
  !define VAR_8 8 ;$8
  !define VAR_9 9 ;$9

  ShowInstDetails nevershow
  ShowUninstDetails nevershow
  XPStyle on


  Var WINPCAP_UNINSTALL ;declare variable for holding the value of a registry key 
  Var WINPCAP_30_OR_LATER ;versions of WinPcap older than 3.0 don't have uninstaller. We use this variable to manage this and call the uninstaller only if present 
  Var WINPCAP_OLD_PROJ_VERSION_DOTTED
  Var WINPCAP_TARGET_OS

;--------------------------------
;General


  ;Name and file
  Name "${WINPCAP_PRODUCT_NAME}"
  OutFile "${WINPCAP_FILE_NAME}"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\WinPcap" ""

;--------------------------------
;User interface Configuration

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "distribution\winpcap_nsis.bmp"
  !define MUI_ABORTWARNING

;--------------------------------
;Installer Pages

  ;Installer
  Page custom "ShowHtmlPage" "" ""
  !insertmacro MUI_PAGE_LICENSE "distribution\license.txt"
  !insertmacro MUI_PAGE_INSTFILES
  !define MUI_FINISHPAGE_TEXT_REBOOT "An old version of WinPcap was present on the system.\nIn case it was older than 2.3, you MUST reboot the system in order for the new version to work properly."
  !insertmacro MUI_PAGE_FINISH

  ;Uninstaller
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH
  
;--------------------------------
;Languages
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Init function: copy the files we need during the installer

  Function .onInit
    InitPluginsDir
    SetSilent normal
    Call IsWinPcapInstalled
    Call GetWindowsVersion
    Pop $WINPCAP_TARGET_OS

    StrCmp $WINPCAP_OLD_PROJ_VERSION_DOTTED "${WINPCAP_PROJ_VERSION_DOTTED}" SameVersionExit

    Call IsUserAdmin
    Pop $R0
    StrCmp $R0 "true" NormalInstallation

    MessageBox MB_ICONEXCLAMATION|MB_OK "${WINPCAP_PRODUCT_NAME} requires administrator privileges to be installed. The installation will be aborted."
    Quit

SameVersionExit:
    MessageBox MB_ICONEXCLAMATION|MB_OK "${WINPCAP_PRODUCT_NAME} is already installed on this machine. The installation will be aborted."
    Quit

NormalInstallation:

    # Drop everything into the $INSTDIR on init.
    # For quick calling at its time
    CreateDirectory $INSTDIR
;    File /oname=$INSTDIR\nsWeb.dll "${NSISDIR}\Plugins\nsWeb.dll"
;    File /oname=$INSTDIR\Internet.dll "${NSISDIR}\Plugins\Internet.dll"
;    File /oname=$INSTDIR\ExecDos.dll "${NSISDIR}\Plugins\ExecDos.dll"
;    File /oname=$INSTDIR\UserInfo.dll "${NSISDIR}\Plugins\UserInfo.dll"
    File /oname=$INSTDIR\WpBann.htm WpBann.htm
;    File /oname=$INSTDIR\winpcap_nsis.bmp distribution\winpcap_nsis.bmp

    # we always set the installer mode to normal to use the page
    # callback functions
  FunctionEnd

;--------------------------------
;Init function for the uninstaller: disable support for silent removal
  Function un.onInit
    InitPluginsDir
    SetSilent normal
    Call un.GetWindowsVersion
    Pop $WINPCAP_TARGET_OS

    Call un.IsUserAdmin
    Pop $R0
    StrCmp $R0 "true" NormalUninstallation

    MessageBox MB_ICONEXCLAMATION|MB_OK "${WINPCAP_PRODUCT_NAME} requires administrator privileges to be uninstalled. The uninstallation will be aborted."
    Quit

NormalUninstallation:
  FunctionEnd



  Function .onInstFailed
    call RollBackInstallation
  FunctionEnd

  Function .onInstSuccess
    call CleanupTempFiles
  FunctionEnd

  Function RollBackInstallation
    call CleanupTempFiles
  FunctionEnd


  Function CleanupTempFiles
;    Delete /REBOOTOK $INSTDIR\nsWeb.dll
;    Delete /REBOOTOK $INSTDIR\Internet.dll
;    Delete /REBOOTOK $INSTDIR\ExecDos.dll
;    Delete /REBOOTOK $INSTDIR\UserInfo.dll
    Delete /REBOOTOK $INSTDIR\WpBann.htm
;    Delete /REBOOTOK $INSTDIR\winpcap_nsis.bmp
  FunctionEnd




;--------------------------------
;web-pages functions

  Function "ShowHtmlPage"

    push $R0
    push $1

    ;Now jump to the copy functions related to this OS
    StrCmp WINPCAP_TARGET_OS "95" LocalBanner
    StrCmp WINPCAP_TARGET_OS "98" LocalBanner
    StrCmp WINPCAP_TARGET_OS "ME" LocalBanner
    StrCmp WINPCAP_TARGET_OS "NT" RemoteBanner
    StrCmp WINPCAP_TARGET_OS "2000" RemoteBanner
    StrCmp WINPCAP_TARGET_OS "XP" RemoteBanner
    StrCmp WINPCAP_TARGET_OS "2003" RemoteBanner
 
RemoteBanner:
	
    Internet::GetUrlCode "www.winpcap.org" "" ${VAR_1}

!insertmacro MUI_HEADER_TEXT "WinPcap Installer" "Welcome to the WinPcap 3.1 Installation Wizard"

    StrCmp $1 "200" remote

LocalBanner:
    nsWeb::ShowWebInPage "$INSTDIR\WpBann.htm"
    Goto end

remote:
    nsWeb::ShowWebInPage "http://www.winpcap.org/install/banner/${WINPCAP_PROJ_VERSION_DOTTED}/banner.htm"

end:

    pop $1
    pop $R0
  FunctionEnd


;--------------------------------
;Main Installer Section
Section "Dummy Section" SecDummy

  ;
  ; First of all, we delete the intermediate files used only by the installer
  ;
  Call CleanupTempFiles
  ;
  ; Now we check if an old WinPcap version is pesent, and we try to remove it
  ;

  ;Assume no reboot by default
  SetRebootFlag false

  ReadRegStr $WINPCAP_UNINSTALL HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinPcapInst" "UninstallString"
  IfErrors WinpcapNotDetected ;if RegKey is unavailable, WinPcap is not present

  StrCpy $WINPCAP_30_OR_LATER "no"

  StrCmp $WINPCAP_30_OR_LATER "no" RemoveOld

  ;3.0 or more recent version detected. Ask the user what to do.
  Messagebox MB_YESNO|MB_ICONINFORMATION "A previous version of WinPcap has been detected on this system and needs to be removed before proceeding. Do you want to uninstall it? (Note: if you click 'No', this installer will abort.)" IDYES RemoveRecent

  Abort "Installation aborted. The old version of WinPcap has not been removed. Click Cancel to Exit."
  Goto EndInstallation

RemoveRecent:
;  ExecWait '$WINPCAP_UNINSTALL' 
  ExecWait '"$INSTDIR\uninstall.exe" _?=$INSTDIR' 
  Goto WinpcapNotDetected

RemoveOld:
  ;If WinPcap is detected, but it's older than 3.0, we overwrite and ask a reboot
  SetRebootFlag true
  Messagebox MB_YESNO|MB_ICONINFORMATION "A version of WinPcap older than 3.0 has been detected on this system and needs to be removed before proceeding. Do you want to uninstall it? (Note: if you click 'No', this installer will abort.)" IDYES WinpcapNotDetected

  SetRebootFlag false
  Abort "Installation aborted. The old version of WinPcap has not been removed. Click Cancel to Exit."
  Goto EndInstallation

WinpcapNotDetected:


  ;
  ;tell the installer to put the start menu stuff under "all users"
  ;this prevents NSIS from having craziness during uninstallation
  ;
  SetShellVarContext all 

  SetOutPath "$INSTDIR"
  
  ;
  ;Copy Files
  ;

  ;Common Files
  File "Distribution\2000\daemon_mgm.exe"
  File "Distribution\2000\NetMonInstaller.exe"
  File "Distribution\2000\npf_mgm.exe"
  File "Distribution\2000\rpcapd.exe"

  ;Move to the system folder, whatever system we are
  SetOutPath "$SYSDIR"

  ;Copy wpcap
  File "Distribution\wpcap.dll"
  File "Distribution\pthreadVC.dll"

  ;Now jump to the copy functions related to this OS
  StrCmp WINPCAP_TARGET_OS "95" CopyFiles9x
  StrCmp WINPCAP_TARGET_OS "98" CopyFiles9x
  StrCmp WINPCAP_TARGET_OS "ME" CopyFiles9x
  StrCmp WINPCAP_TARGET_OS "NT" CopyFilesNT4
  StrCmp WINPCAP_TARGET_OS "2000" CopyFilesNT5
  StrCmp WINPCAP_TARGET_OS "XP" CopyFilesNT5
  StrCmp WINPCAP_TARGET_OS "2003" CopyFilesNT5
  
CopyFilesNT5:
  File "Distribution\2000\packet.dll"
  File "Distribution\2000\wanpacket.dll"
  File /oname=drivers\npf.sys Distribution\2000\npf.sys
  
  ;Run install commands
  ExecDos::exec '$INSTDIR\npf_mgm.exe -r'
  ExecDos::exec '$INSTDIR\daemon_mgm.exe -r'
  ExecDos::exec '$INSTDIR\NetMonInstaller.exe i'

  Goto EndCopy

CopyFilesNT4:
  File "Distribution\NT\packet.dll"
  File /oname=drivers\npf.sys Distribution\NT\npf.sys

  ;Run install commands
  ExecDos::exec '"$INSTDIR\npf_mgm.exe" "-r"'
  ExecDos::exec '"$INSTDIR\daemon_mgm.exe" "-r"'

  Goto EndCopy

CopyFiles9x:
  File "Distribution\9x\packet.dll"
  File "Distribution\9x\npf.vxd"
  Goto EndCopy

EndCopy:

  ;
  ;Store installation folder
  ;
  WriteRegStr HKCU "Software\WinPcap" "" $INSTDIR
 
  ;
  ;Create an "add/remove programs" entry
  ;
  WriteRegStr HKLM \ 
	"Software\Microsoft\Windows\CurrentVersion\Uninstall\WinPcapInst" \ 
	"DisplayName" "WINPCAP_PRODUCT_NAME" 
	
  WriteRegStr HKLM \ 
	"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinPcapInst" \ 
	"UninstallString" "$INSTDIR\uninstall.exe"  

  WriteRegStr HKLM \ 
	"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinPcapInst" \ 
	"Publisher" "CACE Technologies"  

  WriteRegStr HKLM \ 
	"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinPcapInst" \ 
	"HelpLink" "http://www.cacetech.com"  

  WriteRegStr HKLM \ 
	"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinPcapInst" \ 
	"URLInfoAbout" "http://www.winpcap.org"  
  
  WriteRegStr HKLM \ 
	"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinPcapInst" \ 
	"URLUpdateInfo" "http://www.winpcap.org"  

  WriteRegStr HKLM \ 
	"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinPcapInst" \ 
	"VersionMajor" "${WINPCAP_PRJ_MAJOR}"  

  WriteRegStr HKLM \ 
	"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinPcapInst" \ 
	"VersionMinor" "${WINPCAP_PRJ_MINOR}"  

  WriteRegStr HKLM \ 
	"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinPcapInst" \ 
	"DisplayVersion" "${WINPCAP_PRJ_MAJOR}.${WINPCAP_PRJ_MINOR}"  

  SetOutPath $PROFILE
  CreateDirectory "$SMPROGRAMS\WinPcap"

  ;
  ;Create start menu shortcuts
  ;
  Delete "$SMPROGRAMS\WinPcap\WinPcap Web Site.lnk"
  WriteINIStr "$SMPROGRAMS\WinPcap\WinPcap Web Site.url" \
          "InternetShortcut" "URL" "http://www.winpcap.org/"
  CreateShortCut "$SMPROGRAMS\WinPcap\Uninstall WinPcap.lnk" "$INSTDIR\uninstall.exe"

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

EndInstallation:

SectionEnd

;--------------------------------
;Language-related stuff
  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "WinPcap installation section. No panels"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section
Section "Uninstall"

  SetShellVarContext all 

  ;
  ;Delete files
  ;

  ;Get OS version with the GetWindowsVersion function included at the end of the script
  Call un.GetWindowsVersion
  Pop $R0

  ;Move to the system folder, whatever system we are
  SetOutPath "$SYSDIR"

  ;Now jump to the copy functions related to this OS
  StrCmp $3 "95" RmFiles9x
  StrCmp $3 "98" RmFiles9x
  StrCmp $3 "ME" RmFiles9x
  StrCmp $3 "NT" RmFilesNT4
  StrCmp $3 "2000" RmFilesNT5
  StrCmp $3 "XP" RmFilesNT5
  StrCmp $3 "2003" RmFilesNT5
  
RmFilesNT5:
  ;Run uninstall commands
  ExecDos::exec '"$INSTDIR\npf_mgm.exe" "-u"'
  ExecDos::exec '"$INSTDIR\daemon_mgm.exe" "-u"'

  ;Delete files
  Delete /REBOOTOK "$SYSDIR\packet.dll"
  Delete /REBOOTOK "$SYSDIR\wanpacket.dll"
  Delete /REBOOTOK "$SYSDIR\drivers\npf.sys"

  Goto EndRm

RmFilesNT4:
  ;Run uninstall commands
  ExecDos::exec '"$INSTDIR\npf_mgm.exe" "-u"'
  ExecDos::exec '"$INSTDIR\daemon_mgm.exe" "-u"'

  Delete /REBOOTOK "$SYSDIR\packet.dll"
  Delete /REBOOTOK "$SYSDIR\drivers\npf.sys"
  Goto EndRm

RmFiles9x:
  Delete "$SYSDIR\packet.dll"
  Delete "$SYSDIR\npf.vxd"
  Goto EndRm

EndRm:

  ;Delete common files
  Delete /REBOOTOK "$INSTDIR\INSTALL.LOG" ;may be present after removing winpcap 2.3
  Delete /REBOOTOK "$INSTDIR\Uninstall.exe"
  Delete /REBOOTOK "$INSTDIR\daemon_mgm.exe"
  Delete /REBOOTOK "$INSTDIR\NetMonInstaller.exe"
  Delete /REBOOTOK "$INSTDIR\npf_mgm.exe"
  Delete /REBOOTOK "$INSTDIR\rpcapd.exe"
  Delete /REBOOTOK "$SYSDIR\wpcap.dll"

  ;
  ;remove start menu entries
  ;
  SetOutPath "$SMPROGRAMS"
  RMDir /r "$SMPROGRAMS\WinPcap" 

  ; 
  ;delete the installation directory
  ;
  RMDir "$INSTDIR"

  ;
  ;remove registry entries
  ;
  DeleteRegKey /ifempty HKCU "Software\WinPcap"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinPcapInst"

SectionEnd

;--------------------------------
;Installer Version Information
  VIProductVersion "${WINPCAP_PROJ_VERSION_DOTTED}"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${WINPCAP_PRODUCT_NAME}"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" ""
  VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "${WINPCAP_COMPANY_NAME}"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" ""
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "${WINPCAP_LEGAL_COPYRIGHT}"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${WINPCAP_PRODUCT_NAME} installer"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${WINPCAP_PROJ_VERSION_DOTTED}"




;--------------------------------
;--------------------------------
;--------------------------------
;--------------------------------
; GetWindowsVersion Function, coming from nsis manual but slightly
; modified by LD to return NT without number
;
; Based on Yazno's function, http://yazno.tripod.com/powerpimpit/
; Updated by Joost Verburg
;
; Returns on top of stack
;
; Windows Version (95, 98, ME, NT, 2000, XP, 2003)
; or
; '' (Unknown Windows Version)
;
; Usage:
;   Call GetWindowsVersion
;   Pop $R0
;   ; at this point $R0 is "NT 4.0" or whatnot
 
 Function GetWindowsVersion
 
   Push $R0
   Push $R1
 
   ClearErrors
 
   ReadRegStr $R0 HKLM \
   "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion

   IfErrors 0 lbl_winnt
   
   ; we are not NT
   ReadRegStr $R0 HKLM \
   "SOFTWARE\Microsoft\Windows\CurrentVersion" VersionNumber
 
   StrCpy $R1 $R0 1
   StrCmp $R1 '4' 0 lbl_error
 
   StrCpy $R1 $R0 3
 
   StrCmp $R1 '4.0' lbl_win32_95
   StrCmp $R1 '4.9' lbl_win32_ME lbl_win32_98
 
   lbl_win32_95:
     StrCpy $R0 '95'
   Goto lbl_done
 
   lbl_win32_98:
     StrCpy $R0 '98'
   Goto lbl_done
 
   lbl_win32_ME:
     StrCpy $R0 'ME'
   Goto lbl_done
 
   lbl_winnt:
 
   StrCpy $R1 $R0 1
 
   StrCmp $R1 '3' lbl_winnt_x
   StrCmp $R1 '4' lbl_winnt_x
 
   StrCpy $R1 $R0 3
 
   StrCmp $R1 '5.0' lbl_winnt_2000
   StrCmp $R1 '5.1' lbl_winnt_XP
   StrCmp $R1 '5.2' lbl_winnt_2003 lbl_error
 
   lbl_winnt_x:
     StrCpy $R0 'NT'
   Goto lbl_done
 
   lbl_winnt_2000:
     Strcpy $R0 '2000'
   Goto lbl_done
 
   lbl_winnt_XP:
     Strcpy $R0 'XP'
   Goto lbl_done
 
   lbl_winnt_2003:
     Strcpy $R0 '2003'
   Goto lbl_done
 
   lbl_error:
     Strcpy $R0 ''
   lbl_done:
 
   Pop $R1
   Exch $R0
 
 FunctionEnd

;--------------------------------
;Exactly the same of previous function, but the name starts with "un."
;The reason is that the NSIS uninstaller accepts only function that start with "un."
 
 Function un.GetWindowsVersion
 
   Push $R0
   Push $R1
 
   ClearErrors
 
   ReadRegStr $R0 HKLM \
   "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion

   IfErrors 0 lbl_winnt
   
   ; we are not NT
   ReadRegStr $R0 HKLM \
   "SOFTWARE\Microsoft\Windows\CurrentVersion" VersionNumber
 
   StrCpy $R1 $R0 1
   StrCmp $R1 '4' 0 lbl_error
 
   StrCpy $R1 $R0 3
 
   StrCmp $R1 '4.0' lbl_win32_95
   StrCmp $R1 '4.9' lbl_win32_ME lbl_win32_98
 
   lbl_win32_95:
     StrCpy $R0 '95'
   Goto lbl_done
 
   lbl_win32_98:
     StrCpy $R0 '98'
   Goto lbl_done
 
   lbl_win32_ME:
     StrCpy $R0 'ME'
   Goto lbl_done
 
   lbl_winnt:
 
   StrCpy $R1 $R0 1
 
   StrCmp $R1 '3' lbl_winnt_x
   StrCmp $R1 '4' lbl_winnt_x
 
   StrCpy $R1 $R0 3
 
   StrCmp $R1 '5.0' lbl_winnt_2000
   StrCmp $R1 '5.1' lbl_winnt_XP
   StrCmp $R1 '5.2' lbl_winnt_2003 lbl_error
 
   lbl_winnt_x:
     StrCpy $R0 'NT'
   Goto lbl_done
 
   lbl_winnt_2000:
     Strcpy $R0 '2000'
   Goto lbl_done
 
   lbl_winnt_XP:
     Strcpy $R0 'XP'
   Goto lbl_done
 
   lbl_winnt_2003:
     Strcpy $R0 '2003'
   Goto lbl_done
 
   lbl_error:
     Strcpy $R0 ''
   lbl_done:
 
   Pop $R1
   Exch $R0
 
 FunctionEnd

;--------------------------------
;this function checks if WinPcap is installed, and stores the internal version
; into WINPCAP_OLD_PROJ_VERSION_DOTTED. 0.0.0.0 means no older version, and
; 1.0.0.0 means very old version (< 2.3)
Function IsWinPcapInstalled

  ${Locate} "$SYSDIR" "/M=packet.dll" 'PacketFound'
  IfErrors NoPacket

  goto IWINT_End
 
NoPacket:
; this means no old version.
  StrCpy $WINPCAP_OLD_PROJ_VERSION_DOTTED "0.0.0.0"
  goto IWINT_End

IWINT_EnD: 

FunctionEnd  

; this callback is called when packet.dll is found. It stores the version of the dll 
; into  WINPCAP_OLD_PROJ_VERSION_DOTTED
Function PacketFound

  Push $R0
  Push $R1
  Push $R2
  Push $R3
  Push $R4
  Push $R5

  GetDLLVersion "$SYSDIR\packet.dll" $R0 $R1
  IfErrors IWINT_Exit 
 
  IntOp $R2 $R0 / 0x00010000
  IntOp $R3 $R0 & 0x0000FFFF
  IntOp $R4 $R1 / 0x00010000
  IntOp $R5 $R1 & 0x0000FFFF

  StrCpy $WINPCAP_OLD_PROJ_VERSION_DOTTED "$R2.$R3.$R4.R5"

  Goto PF_End

IWINT_Exit:
  StrCpy $WINPCAP_OLD_PROJ_VERSION_DOTTED "1.0.0.0"


PF_End:

  pop $R5
  pop $R4
  pop $R3
  pop $R2
  pop $R1
  pop $R0

FunctionEnd  

; Author: Lilla (lilla@earthlink.net) 2003-06-13
; function IsUserAdmin uses plugin \NSIS\PlusgIns\UserInfo.dll
; This function is based upon code in \NSIS\Contrib\UserInfo\UserInfo.nsi
; This function was tested under NSIS 2 beta 4 (latest CVS as of this writing).
;
; Usage:
;   Call IsUserAdmin
;   Pop $R0   ; at this point $R0 is "true" or "false"
;
Function IsUserAdmin
Push $R0
Push $R1
Push $R2

ClearErrors
UserInfo::GetName
IfErrors Win9x
Pop $R1
UserInfo::GetAccountType
Pop $R2

StrCmp $R2 "Admin" 0 Continue
; Observation: I get here when running Win98SE. (Lilla)
; The functions UserInfo.dll looks for are there on Win98 too,
; but just don't work. So UserInfo.dll, knowing that admin isn't required
; on Win98, returns admin anyway. (per kichik)
; MessageBox MB_OK 'User "$R1" is in the Administrators group'
StrCpy $R0 "true"
Goto Done

Continue:
; You should still check for an empty string because the functions
; UserInfo.dll looks for may not be present on Windows 95. (per kichik)
StrCmp $R2 "" Win9x
StrCpy $R0 "false"
;MessageBox MB_OK 'User "$R1" is in the "$R2" group'
Goto Done

Win9x:
; comment/message below is by UserInfo.nsi author:
; This one means you don't need to care about admin or
; not admin because Windows 9x doesn't either
;MessageBox MB_OK "Error! This DLL can't run under Windows 9x!"
StrCpy $R0 "true"

Done:
;MessageBox MB_OK 'User= "$R1"  AccountType= "$R2"  IsUserAdmin= "$R0"'

Pop $R2
Pop $R1
Exch $R0
FunctionEnd

; Author: Lilla (lilla@earthlink.net) 2003-06-13
; function IsUserAdmin uses plugin \NSIS\PlusgIns\UserInfo.dll
; This function is based upon code in \NSIS\Contrib\UserInfo\UserInfo.nsi
; This function was tested under NSIS 2 beta 4 (latest CVS as of this writing).
;
; Usage:
;   Call IsUserAdmin
;   Pop $R0   ; at this point $R0 is "true" or "false"
;
Function un.IsUserAdmin
Push $R0
Push $R1
Push $R2

ClearErrors
UserInfo::GetName
IfErrors Win9x
Pop $R1
UserInfo::GetAccountType
Pop $R2

StrCmp $R2 "Admin" 0 Continue
; Observation: I get here when running Win98SE. (Lilla)
; The functions UserInfo.dll looks for are there on Win98 too,
; but just don't work. So UserInfo.dll, knowing that admin isn't required
; on Win98, returns admin anyway. (per kichik)
; MessageBox MB_OK 'User "$R1" is in the Administrators group'
StrCpy $R0 "true"
Goto Done

Continue:
; You should still check for an empty string because the functions
; UserInfo.dll looks for may not be present on Windows 95. (per kichik)
StrCmp $R2 "" Win9x
StrCpy $R0 "false"
;MessageBox MB_OK 'User "$R1" is in the "$R2" group'
Goto Done

Win9x:
; comment/message below is by UserInfo.nsi author:
; This one means you don't need to care about admin or
; not admin because Windows 9x doesn't either
;MessageBox MB_OK "Error! This DLL can't run under Windows 9x!"
StrCpy $R0 "true"

Done:
;MessageBox MB_OK 'User= "$R1"  AccountType= "$R2"  IsUserAdmin= "$R0"'

Pop $R2
Pop $R1
Exch $R0
FunctionEnd


   






