import os
import utils
from SCons.Environment import Environment
from SCons.Script import Exit

def checkPython(context):
    context.Message("Check for python..")
    context.Result(0)
    return False

def checkStaticSDL(context):
    context.Message("Checking for static SDL... ")
    env = context.env

    try:
        utils.safeParseConfig(env, 'sdl-config --static-libs --cflags')
        env.Append(CPPDEFINES = ['USE_SDL'])
    except Exception:
        context.Result(utils.colorResult(0))
        return 0
    context.Result(utils.colorResult(1))
    return 1


def checkStaticAllegro4(context):
    context.Message("Check for static Allegro4..")
    context.Result(0)
    return False

def checkStaticAllegro5(context):
    context.Message("Check for static Allegro5..")
    context.Result(0)
    return False

def getStaticEnvironment():
    environment = Environment(ENV = os.environ)
    peg_color = 'light-cyan'
    environment['PAINTOWN_PLATFORM'] = ['osx']
    environment['PAINTOWN_USE_PRX'] = False
    environment['PAINTOWN_TESTS'] = {'CheckPython': checkPython}
    environment['PAINTOWN_COLORIZE'] = utils.colorize
    environment['PAINTOWN_NETWORKING'] = True
    environment['LIBS'] = []
    environment['PEG_MAKE'] = "%s %s" % (utils.colorize('Creating peg parser', peg_color), utils.colorize('$TARGET', 'light-blue'))
    environment.Append(BUILDERS = {'Peg' : utils.pegBuilder(environment)})
    environment.Append(CPPPATH = ['#src', '#src/util/network/hawknl'])
    environment['LINKCOM'] = '$CXX $LINKFLAGS $SOURCES -Wl,--start-group $ARCHIVES $_LIBDIRFLAGS $_LIBFLAGS -Wl,--end-group -o $TARGET'

    # Default preference for a graphics renderer / input system
    backends = ['SDL', 'Allegro4', 'Allegro5']

    if utils.useAllegro4():
        backends = ['Allegro4', 'SDL', 'Allegro5']

    if utils.useAllegro5():
        backends = ['Allegro5', 'SDL', 'Allegro4']

    environment.ParseConfig('freetype-config --libs --cflags')
    environment.ParseConfig('libpng-config --libs --cflags')

    if utils.useLLVM():
        llvm(environment)

    custom_tests = {"CheckCompiler": utils.checkCompiler,
                    "CheckSDL" : checkStaticSDL,
                    "CheckAllegro4" : checkStaticAllegro4,
                    "CheckAllegro5" : checkStaticAllegro5}

    return utils.configure(environment, backends, custom_tests)
