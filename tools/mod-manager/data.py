def windowsPath():
    import ctypes, os
    from ctypes import wintypes, windll

    CSIDL_APPDATA = 26

    _SHGetFolderPath = windll.shell32.SHGetSpecialFolderPathA
    #_SHGetFolderPath.argtypes = [wintypes.HWND,
    #                             wintypes.LPCWSTR,
    #                             ctypes.c_int,
    #                             ctypes.c_byte]


    #path_buf = wintypes.create_unicode_buffer(wintypes.MAX_PATH)
    path_buf = wintypes.create_string_buffer(wintypes.MAX_PATH)
    result = _SHGetFolderPath(0, path_buf, CSIDL_APPDATA, 0)
    return os.path.join(path_buf.value, 'paintown')

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
