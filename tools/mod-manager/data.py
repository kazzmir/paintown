def windowsPath():
    import ctypes
    from ctypes import wintypes, windll

    CSIDL_COMMON_APPDATA = 35

    _SHGetFolderPath = windll.shell32.SHGetFolderPathW
    _SHGetFolderPath.argtypes = [wintypes.HWND,
                                 ctypes.c_int,
                                 wintypes.HANDLE,
                                 wintypes.DWORD, wintypes.LPCWSTR]


    path_buf = wintypes.create_unicode_buffer(wintypes.MAX_PATH)
    result = _SHGetFolderPath(0, CSIDL_COMMON_APPDATA, 0, 0, path_buf)
    return path_buf.value

def unixPath():
    import os
    return os.environ['HOME'] + '/.paintown'

def isWindows():
    import re
    import sys
    return "win32" in sys.platform

def modPath():
    if isWindows():
        return windowsPath()
    return unixPath()
