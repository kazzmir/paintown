import os
import utils
from SCons.Environment import Environment
from SCons.Script import Exit

def wii(env):
    bin_path = "%s/bin" % os.environ['DEVKITPPC']
    ogc_bin_path = "%s/libogc/bin" % os.environ['DEVKITPRO']
    prefix = 'powerpc-eabi-'
    def setup(x):
        return '%s%s' % (prefix, x)
    env['CC'] = setup('gcc')
    env['LD'] = setup('ld')
    env['CXX'] = setup('g++')
    env['AS'] = setup('as')
    env['AR'] = setup('ar')
    env['OBJCOPY'] = setup('objcopy')
    if utils.isWindows():
        env.Append(CPPPATH = ["%s/libogc/include" % os.environ['DEVKITPRO'],
            "%s/libogc/include/SDL" % os.environ['DEVKITPRO'],
            "%s/libogc/include/freetype2" % os.environ['DEVKITPRO']])
        env.Append(LIBPATH = ["%s/libogc/lib" % os.environ['DEVKITPRO'],
            "%s/libogc/lib/wii" % os.environ['DEVKITPRO']])
        env.Append(LIBS = ['SDL', 'SDL_image', 'SDL_mixer', 'png', 'freetype', 'z'])
    else:
        env.Append(CPPPATH = ["%s/libogc/include" % os.environ['DEVKITPRO']])
    env.Append(CPPDEFINES = ['GEKKO', 'WII'])
    flags = ['-mrvl', '-mcpu=750', '-meabi', '-mhard-float']
    env.Append(CCFLAGS = flags)
    env.Append(CXXFLAGS = flags)
    env.Append(LINKFLAGS = flags)
    # env.Append(CPPPATH = ['#src/wii'])
    env['LINKCOM'] = '$CXX $LINKFLAGS -Wl,--start-group $ARCHIVES $SOURCES $_LIBDIRFLAGS $_LIBFLAGS -Wl,--end-group -o $TARGET'
    env.Append(LIBS = ['wiiuse', 'wiikeyboard', 'iberty', 'bte', 'fat', 'ogc', 'm'])
    # os.environ['PATH'] = "%s:%s:%s" % (bin_path, ogc_bin_path, os.environ['PATH'])
    env.PrependENVPath('PATH', bin_path)
    env.PrependENVPath('PATH', ogc_bin_path)
    env.Append(CPPDEFINES = ['USE_SDL_MAIN'])
    utils.safeParseConfig(env, 'sdl-config --cflags --libs')
    utils.safeParseConfig(env, 'freetype-config --libs --cflags')
    utils.safeParseConfig(env, 'libpng-config --libs --cflags')
    return env

def checkPython(context):
    context.Result(0)
    return 0

def getEnvironment():
    import utils
    environment = Environment(ENV = os.environ)
    environment['PAINTOWN_PLATFORM'] = ['wii', 'sdl']
    peg_color = 'light-cyan'
    environment['PAINTOWN_BACKEND'] = 'sdl'
    environment['PAINTOWN_USE_PRX'] = False
    environment['PAINTOWN_TESTS'] = {'CheckPython': checkPython}
    environment['PAINTOWN_COLORIZE'] = utils.colorize
    environment['PAINTOWN_NETWORKING'] = False
    environment['LIBS'] = []
    environment['PEG_MAKE'] = "%s %s" % (utils.colorize('Creating peg parser', peg_color), utils.colorize('$TARGET', 'light-blue'))
    environment.Append(BUILDERS = {'Peg' : utils.pegBuilder(environment)})
    environment.Append(CPPPATH = ['#src', '#src/util/network/hawknl'])
    environment.Append(CPPDEFINES = ['USE_SDL'])

    return utils.lessVerbose(wii(environment))
