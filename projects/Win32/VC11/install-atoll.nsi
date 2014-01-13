;--------------------------------
!define WANT_LIBS ; Want icu xerces xalan libs ?
;!define WANT_VC90 ; Want Microsoft VC90 ?
!define WANT_TEST ; Want Unittest ?
!define WANT_PLUGIN_XMLEXEC ; Want plugin xmlexec
!define WANT_PLUGIN_XSLT ; Want plugin xslt
!define PRODUCT_NAME "atoll"
!define PRODUCT_TITLE_SHORT "atoll"
!define PRODUCT_TITLE "atoll"
!define BASE_DIR "$%RepDev%"
!define VCREDISTDIR "$%RepDev%\lib\VCredist_x86"
!define BDB_DIR      "$%BERKELEYDBROOT%\build_windows\Win32\Release"
!define ICU_DIR      "$%ICUROOT%\bin"
!define XERCES_DIR   "$%XERCESCROOT%\bin"
!define XALANC_DIR   "$%XALANCROOT%\bin"
!define PRODUCT_PUBLISHER "Atoll Digital Library"
!define PRODUCT_WEB_SITE "http://www.atoll-digital-library.org/"

!define PRODUCT_VERSION "1.0"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

;--------------------------------
;Include Modern UI
!include "MUI.nsh"

;--------------------------------
;General

; Nom affiché par l'installateur
Name "${PRODUCT_TITLE_SHORT}"

; Fichier de sortie
OutFile "${BASE_DIR}\${PRODUCT_NAME}\win\Install.exe"

;Default installation folder
InstallDir "$PROGRAMFILES\Atoll-digital-library\${PRODUCT_NAME}"
;Get installation folder from registry if available
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""

ShowInstDetails show
ShowUnInstDetails show
BrandingText " "
XPStyle on

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

;--------------------------------
;Pages

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!define MUI_LICENSEPAGE_RADIOBUTTONS
!insertmacro MUI_PAGE_LICENSE "$(myLicenseData)" ;myLicenseData est défini plus bas
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
;Finish
;!define MUI_FINISHPAGE_RUN "$INSTDIR\${PRODUCT_NAME}.exe"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\doc\README.txt"
!insertmacro MUI_PAGE_FINISH

;--------------------------------
; Uninstaller pages

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"
; License data
LicenseLangString myLicenseData ${LANG_ENGLISH} "${BASE_DIR}\${PRODUCT_NAME}\doc\LICENSE"
LicenseLangString myLicenseData ${LANG_FRENCH} "${BASE_DIR}\${PRODUCT_NAME}\doc\LICENSE"

;--------------------------------
;Reserve Files

;These files should be inserted before other files in the data block
;Keep these lines before any File command
!insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
;Installer Sections

Section "Exe" SEC01
  SetOutPath "$INSTDIR"
  ;SetOverwrite ifnewer
  ;File "/oname=${PRODUCT_NAME}.exe" "${BASE_DIR}\${PRODUCT_NAME}\win\release\${PRODUCT_NAME}_console.exe"
  File "${BASE_DIR}\${PRODUCT_NAME}\win\release\${PRODUCT_NAME}_console.exe"
  File "${BASE_DIR}\${PRODUCT_NAME}\win\release\${PRODUCT_NAME}_engine.dll"
  ; Plugins
!ifdef WANT_PLUGIN_XMLEXEC
  File "${BASE_DIR}\${PRODUCT_NAME}\win\release\${PRODUCT_NAME}_plugin_xmlexec.dll"
!endif
!ifdef WANT_PLUGIN_XSLT
  File "${BASE_DIR}\${PRODUCT_NAME}\win\release\${PRODUCT_NAME}_plugin_xslt.dll"
!endif
  CreateDirectory "$INSTDIR\data_db"
  CreateDirectory "$INSTDIR\data_dst"
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\Atoll-digital-library"
  CreateShortCut "$SMPROGRAMS\Atoll-digital-library\${PRODUCT_TITLE}.lnk" "$INSTDIR\${PRODUCT_NAME}_console.exe"
  ; Unittest shortcut
!ifdef WANT_TEST
  CreateShortCut "$INSTDIR\run_unittest.lnk" "$INSTDIR\${PRODUCT_NAME}_console.exe" "-vv -j test -s ./data_unittest/ -d ./data_dst/ -h ./data_unittest/ -l ./data_unittest/_atoll.log" "$INSTDIR\${PRODUCT_NAME}_console.exe" 0
!endif
  ; Plugin xmlexec shortcut
!ifdef WANT_PLUGIN_XMLEXEC
  CreateShortCut "$INSTDIR\run_plugin_xmlexec.lnk" "$INSTDIR\${PRODUCT_NAME}_console.exe" "-vv -s ./data_unittest/ -d ./data_dst/ -h ./data_unittest/ -l ./data_unittest/_atoll.log -x ./data_unittest/xmlexec_req.xml" "$INSTDIR\${PRODUCT_NAME}_console.exe" 0
  CreateShortCut "$INSTDIR\run_index_basile.lnk" "$INSTDIR\${PRODUCT_NAME}_console.exe" "-vv -j indexer -s %RepBase%\col\ColBasile\Xml\ -d ./data_dst/ -h ./data_db/ -l ./_atoll.log" "$INSTDIR\${PRODUCT_NAME}_console.exe" 0
!endif
SectionEnd

!ifdef WANT_LIBS
Section "Lib" SEC02
  SetOutPath "$INSTDIR"
  File "${BDB_DIR}\libdb53.dll"
  File "${ICU_DIR}\icudt49.dll"
  File "${ICU_DIR}\icuin49.dll"
  File "${ICU_DIR}\icuio49.dll"
  File "${ICU_DIR}\icuuc49.dll"
  File "${XERCES_DIR}\xerces-c_3_1.dll"
  File "${XALANC_DIR}\Xalan-C_1_11.dll"
  File "${XALANC_DIR}\XalanMessages_1_11.dll"
SectionEnd
!endif

Section "Doc" SEC03
  SetOutPath "$INSTDIR\doc"
  File "/oname=LICENSE.txt" "${BASE_DIR}\${PRODUCT_NAME}\doc\LICENSE"
  File "/oname=README.txt" "${BASE_DIR}\${PRODUCT_NAME}\doc\README"
  SetOutPath "$INSTDIR"
SectionEnd

!ifdef WANT_VC90
Section "VCredist" SEC04
  SetOutPath "$INSTDIR\Microsoft.VC90.CRT"
  File "${VCREDISTDIR}\Microsoft.VC90.CRT\Microsoft.VC90.CRT.manifest"
  File "${VCREDISTDIR}\Microsoft.VC90.CRT\msvcm90.dll"
  File "${VCREDISTDIR}\Microsoft.VC90.CRT\msvcp90.dll"
  File "${VCREDISTDIR}\Microsoft.VC90.CRT\msvcr90.dll"
  SetOutPath "$INSTDIR"
SectionEnd
!endif

Section "Config" SEC05
  SetOutPath "$INSTDIR\config"
  File "${BASE_DIR}\${PRODUCT_NAME}\config\atoll_indexer_config_default.xml"
  File "${BASE_DIR}\${PRODUCT_NAME}\config\atoll_recordbreaker_config_default.xml"
  SetOutPath "$INSTDIR"
SectionEnd

Section "Dtd" SEC06
  SetOutPath "$INSTDIR\dtd"
  File "${BASE_DIR}\${PRODUCT_NAME}\dtd\atoll_indexer_config.dtd"
  File "${BASE_DIR}\${PRODUCT_NAME}\dtd\atoll_recordbreaker_config.dtd"
!ifdef WANT_PLUGIN_XMLEXEC
  File "${BASE_DIR}\${PRODUCT_NAME}\dtd\atoll_plugin_xmlexec.dtd"
!endif
  SetOutPath "$INSTDIR"
SectionEnd

!ifdef WANT_TEST
Section "Unittest" SEC07
  SetOutPath "$INSTDIR\data_unittest"
  File "${BASE_DIR}\${PRODUCT_NAME}\data_unittest\*.fwb"
  File "${BASE_DIR}\${PRODUCT_NAME}\data_unittest\xslt_test.html"
  File "${BASE_DIR}\${PRODUCT_NAME}\data_unittest\test_xhtml.xhtml"
  File "${BASE_DIR}\${PRODUCT_NAME}\data_unittest\xslt_test_highwords.html"
  File "${BASE_DIR}\${PRODUCT_NAME}\data_unittest\xslt_test_identity.html"
  File "${BASE_DIR}\${PRODUCT_NAME}\data_unittest\*.xml"
  File "${BASE_DIR}\${PRODUCT_NAME}\data_unittest\*.xsl"
  SetOutPath "$INSTDIR\xsl"
  File "${BASE_DIR}\${PRODUCT_NAME}\xsl\cppunit_report.xsl"
  SetOutPath "$INSTDIR"
SectionEnd
!endif

;Section -AdditionalIcons
;  ; Site internet
;  WriteIniStr "$INSTDIR\${PRODUCT_PUBLISHER}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
;  CreateShortCut "$SMPROGRAMS\${PRODUCT_PUBLISHER}\${PRODUCT_PUBLISHER}.lnk" "$INSTDIR\${PRODUCT_PUBLISHER}.url"
;SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\${PRODUCT_NAME}.exe"
  ;WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_PUBLISHER}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\${PRODUCT_NAME}.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLPubSite" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

;Choix de la langue d'installation
Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

;Confirmation de désinstallation
;Function un.onInit
  ;!insertmacro MUI_UNGETLANGUAGE ;Choix de la langue de désinstallation
  ;MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Êtes-vous certains de vouloir désinstaller totalement $(^Name) et tous ses composants ?" IDYES +2
  ;Abort
;FunctionEnd

;Désinstallation
Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.exe"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\*.log"
  Delete "$INSTDIR\*.txt"
  Delete "$INSTDIR\icu*.dll"
  Delete "$INSTDIR\xerces*.dll"
  Delete "$INSTDIR\doc\*.*"
  RMDir "$INSTDIR\doc"
  Delete "$INSTDIR\Microsoft.VC90.CRT\*.*"
  RMDir "$INSTDIR\Microsoft.VC90.CRT"

  SetShellVarContext all
  Delete "$SMPROGRAMS\Atoll-digital-library\${PRODUCT_TITLE}.lnk"

  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd

;Confirmation de désinstallation terminée
;Function un.onUninstSuccess
;  HideWindow
;  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) a été désinstallé avec succès de votre ordinateur."
;FunctionEnd
