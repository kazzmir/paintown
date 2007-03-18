Name "Paintown"

OutFile "paintown-1.0.exe"

InstallDir $PROGRAMFILES\Paintown

Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

Section "Paintown (required)"
	SetOutPath $INSTDIR
	File "..\paintown.exe"
	File "..\alleg42.dll"
	File "..\pthreadGC2.dll"
	File "..\freetype6.dll"
	File "..\zlib1.dll"
	File "..\libpng13.dll"
	File /r /x .svn "..\data"
	WriteUninstaller "uninstall.exe"
SectionEnd

Section "Start Menu Shortcuts"
	CreateDirectory "$SMPROGRAMS\Games\Paintown"
	CreateShortCut "$SMPROGRAMS\Games\Paintown\Uninstall.lnk" "$INSTDIR\uninstall.exe" "$INSTDIR\uninstall.exe" 0
	CreateShortCut "$SMPROGRAMS\Games\Paintown\Paintown.lnk" "$INSTDIR\paintown.exe" "$INSTDIR\paintown.exe" 0
SectionEnd

Section "Uninstall"
	RMDIR /r "$SMPROGRAMS\Games\Paintown"
	RMDIR /r "$INSTDIR"
SectionEnd
