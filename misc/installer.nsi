Name "Paintown"

OutFile "paintown-win32-3.6.1.exe"
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
	File "..\scripting.txt"
	File "..\README"
	File "..\LEGAL"
	File "..\LICENSE"
	File "..\misc\run_level_editor.bat"
	File "..\misc\run_animator_editor.bat"
	File "..\misc\zlib.pyd"
    # WARNING: its easier to include both allegro and SDL rather than
    # figure out what type of build we made. Maybe fix this in the future.
	File "..\misc\dlls\alleg42.dll"
	File "..\misc\dlls\SDL.dll"
	File "..\misc\dlls\libpthread-2.dll"
	File "..\misc\dlls\freetype6.dll"
	File "..\misc\dlls\zlib1.dll"
	File "..\misc\dlls\libpng13.dll"
	File "..\misc\dlls\libpng3.dll"
	File "..\misc\dlls\regex2.dll"
	File "..\misc\dlls\mingwm10.dll"
	File "..\misc\dlls\python26.dll"
	File "..\misc\dlls\msvcr71.dll"
	File "..\editor\editor.jar"
	SetOutPath $INSTDIR\data
	File /r /x .svn /x graveyard /x *~ "..\data\*.*"
	SetOutPath $INSTDIR
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
