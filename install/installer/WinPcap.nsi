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

;--------------------------------
;Project definitions

  !define PRJ_MAJOR "3"
  !define PRJ_MINOR "1"
  
  ;Default installation folder
  InstallDir "$PROGRAMFILES\WinPcap"

  ;This includes the modern user interface
  !include "MUI.nsh"

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
  XPStyle off

  Var WINPCAP_UNINSTALL ;declare variable for holding the value of a registry key 
  Var WINPCAP_30_OR_LATER ;versions of WinPcap older than 3.0 don't have uninstaller. We use this variable to manage this and call the uninstaller only if present 

;--------------------------------
;General

  ;Names
  !define PRJ_NAME "WinPcap ${PRJ_MAJOR}.${PRJ_MINOR}"
  !define PRJ_FILE_NAME "WinPcap_${PRJ_MAJOR}_${PRJ_MINOR}.exe"

  ;Name and file
  Name "${PRJ_NAME}"
  OutFile "${PRJ_FILE_NAME}"
  
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
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Init function: copy the files we need during the installer

  Function .onInit
	 InitPluginsDir
	  # Drop everything into the $INSTDIR on init.
	  # For quick calling at its time
	  File /oname=$INSTDIR\nsWeb.dll "${NSISDIR}\Plugins\nsWeb.dll"
	  File /oname=$INSTDIR\Internet.dll "${NSISDIR}\Plugins\Internet.dll"
	  File /oname=$INSTDIR\ExecDos.dll "${NSISDIR}\Plugins\ExecDos.dll"
	  File /oname=$INSTDIR\WpBann.htm WpBann.htm
	  File /oname=$INSTDIR\winpcap_nsis.bmp distribution\winpcap_nsis.bmp
  FunctionEnd

;--------------------------------
;web-pages functions

  Function "ShowHtmlPage"

	  Internet::GetUrlCode "www.winpcap.org" "" ${VAR_3}

	  !insertmacro MUI_HEADER_TEXT "WinPcap Installer" "Welcome to the WinPcap 3.1 Installation Wizard"

	  StrCmp $3 "200" remote

	  nsWeb::ShowWebInPage "$PLUGINSDIR\WpBann.htm"
;Messagebox MB_OK|MB_ICONINFORMATION "pippo"
	  Goto end

remote:
	  nsWeb::ShowWebInPage "http://www.winpcap.org/install/banner/3.1/banner.htm"  

end:
  FunctionEnd


;--------------------------------
;Small function used to detect when a pre 3.0 version is present
Function UninstallerFound
  StrCpy $WINPCAP_30_OR_LATER "yes"
FunctionEnd

;--------------------------------
;Main Installer Section
Section "Dummy Section" SecDummy

  ;
  ; First of all, we delete the intermediate files used only by the installer
  ;
  Delete $INSTDIR\nsWeb.dll
  Delete $INSTDIR\Internet.dll
  Delete $INSTDIR\ExecDos.dll
  Delete $INSTDIR\WpBann.htm
  Delete $INSTDIR\winpcap_nsis.bmp

  ;
  ; Now we check if an old WinPcap version is pesent, and we try to remove it
  ;

  ;Assume no reboot by default
  SetRebootFlag false

  ReadRegStr $WINPCAP_UNINSTALL HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinPcapInst" "UninstallString"
  IfErrors WinpcapNotDetected ;if RegKey is unavailable, WinPcap is not present

  StrCpy $WINPCAP_30_OR_LATER "no"

  ${Locate} "$INSTDIR" "/M=npf_mgm.exe" "UninstallerFound"
  
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

  ;Get OS version with the GetWindowsVersion function included at the end of the script
  Call GetWindowsVersion
  Pop $R0

  ;Now jump to the copy functions related to this OS
  StrCmp $3 "95" CopyFiles9x
  StrCmp $3 "98" CopyFiles9x
  StrCmp $3 "ME" CopyFiles9x
  StrCmp $3 "NT" CopyFilesNT4
  StrCmp $3 "2000" CopyFilesNT5
  StrCmp $3 "XP" CopyFilesNT5
  StrCmp $3 "2003" CopyFilesNT5
  
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
	"DisplayName" "WinPcap 3.1" 
	
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
	"VersionMajor" "3"  

  WriteRegStr HKLM \ 
	"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinPcapInst" \ 
	"VersionMinor" "1"  

  WriteRegStr HKLM \ 
	"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinPcapInst" \ 
	"DisplayVersion" "3.1"  

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
  Delete "$SYSDIR\packet.dll"
  Delete "$SYSDIR\wanpacket.dll"
  Delete "$SYSDIR\drivers\npf.sys"

  Goto EndRm

RmFilesNT4:
  ;Run uninstall commands
  ExecDos::exec '"$INSTDIR\npf_mgm.exe" "-u"'
  ExecDos::exec '"$INSTDIR\daemon_mgm.exe" "-u"'

  Delete "$SYSDIR\packet.dll"
  Delete "$SYSDIR\drivers\npf.sys"
  Goto EndRm

RmFiles9x:
  Delete "$SYSDIR\packet.dll"
  Delete "$SYSDIR\npf.vxd"
  Goto EndRm

EndRm:

  ;Delete common files
  Delete "$INSTDIR\INSTALL.LOG" ;may be present after removing winpcap 2.3
  Delete "$INSTDIR\Uninstall.exe"
  Delete "$INSTDIR\daemon_mgm.exe"
  Delete "$INSTDIR\NetMonInstaller.exe"
  Delete "$INSTDIR\npf_mgm.exe"
  Delete "$INSTDIR\rpcapd.exe"

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
;Installer Version Iformation
  VIProductVersion "3.1.0.0"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "WinPcap"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" ""
  VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "CACE Technologies"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" ""
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© 2005 CACE Technologies"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "WinPcap installer"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "3.1.0.0"


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
