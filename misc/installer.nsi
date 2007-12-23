Name "Paintown"

OutFile "paintown-win32-2.2.exe"

InstallDir $PROGRAMFILES\Paintown

Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

Section "Paintown (required)"
	SetOutPath $INSTDIR
	File "..\paintown.exe"
	File "..\README"
	File "..\misc\alleg42.dll"
	File "..\misc\pthreadGC2.dll"
	File "..\misc\freetype6.dll"
	File "..\misc\zlib1.dll"
	File "..\misc\libpng13.dll"
	File "..\editor\editor.jar"
	File /r /x .svn "..\data"
	WriteUninstaller "uninstall.exe"
SectionEnd

Section "Start Menu Shortcuts"
	CreateDirectory "$SMPROGRAMS\Games\Paintown"
	CreateShortCut "$SMPROGRAMS\Games\Paintown\Uninstall.lnk" "$INSTDIR\uninstall.exe"
	CreateShortCut "$SMPROGRAMS\Games\Paintown\Paintown.lnk" "$INSTDIR\paintown.exe"
SectionEnd

Section "Uninstall"
	RMDIR /r "$SMPROGRAMS\Games\Paintown"
	RMDIR /r "$INSTDIR"
SectionEnd
