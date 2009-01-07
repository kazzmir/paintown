Name "Paintown"

OutFile "paintown-win32-3.2.exe"
Icon "icon.ico"
WindowIcon on

InstallDir $PROGRAMFILES\Paintown
UninstallIcon "icon.ico"

Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

Section "Paintown (required)"
	SetOutPath $INSTDIR
	File "..\paintown.exe"
	File "..\scripts.zip"
	File "..\README"
	File "..\LEGAL"
	File "..\LICENSE"
	File "..\misc\alleg42.dll"
	File "..\misc\zlib.pyd"
	File "..\misc\pthreadGC2.dll"
	File "..\misc\freetype6.dll"
	File "..\misc\zlib1.dll"
	File "..\misc\libpng13.dll"
	File "..\misc\mingwm10.dll"
	File "..\misc\python24.dll"
	File "..\misc\msvcr71.dll"
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
