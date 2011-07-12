from SCons.Script import ARGUMENTS

def noColors():
    try:
        return int(ARGUMENTS['colors']) == 0
    except KeyError:
        return False

def xterm_color(string, color):
    colors = {'none': "0",
              'black': "0;30",
              'red': "0;31",
              'green': "0;32",
              'brown': "0;33",
              'blue': "0;34",
              'purple': "0;35",
              'cyan': "0;36",
              'light-gray': "0;37",
              'dark-gray': "1:30",
              'light-red': "1;31",
              'light-green': "1;32",
              'yellow': "1;33",
              'light-blue': "1;34",
              'light-purple': "1;35",
              'light-cyan': "1;36",
              'white': "1;37"}
    return "\033[%sm%s\033[0m" % (colors[color], string)

def isPlatform(platform):
    import sys
    return platform in sys.platform

def isWindows():
    return isPlatform("win32")

# todo: figure out when we are on an xterm
def isXterm():
    # assume linux and osx are ok
    return not isWindows()

def colorize(string, color):
    if noColors():
        return string
    if isXterm():
        return xterm_color(string, color)
    return string

def colorResult(what):
    if what != 0:
        return colorize('yes', 'light-green')
    else:
        return colorize('no', 'light-red')
