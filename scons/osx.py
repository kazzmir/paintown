import os
import utils
from SCons.Environment import Environment
from SCons.Script import Exit
from SCons.Script import Action

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

def checkStaticOgg(context):
    context.Message("Checking for ogg and vorbis... ")
    tmp = context.env.Clone()
    env = context.env
    env['HAVE_OGG'] = True
    env.Append(CPPDEFINES = ['HAVE_OGG'])
    (ok, stuff) = context.TryAction(Action("pkg-config --version"))
    if ok:
        try:
            utils.safeParseConfig(env, 'pkg-config vorbisfile --cflags')
            # Strip off the -L part
            libdir = utils.readExec('pkg-config vorbisfile --libs-only-L')[2:].rstrip()
            # Hack to hardcode these libraries
            vorbisfile = env.Install('misc', "%s/libvorbisfile.a" % libdir)
            ogg = env.Install('misc', "%s/libogg.a" % libdir)
            vorbis = env.Install('misc', "%s/libvorbis.a" % libdir)
            env.Append(LIBS = [vorbisfile, ogg, vorbis])
        except OSError:
            context.sconf.env = tmp
            context.Result(utils.colorResult(0))
            return 0
    
    main = 'int main(int argc, char ** argv)'
    try:
        if env['HAVE_SDL_MAIN']:
         main = 'int SDL_main(int argc, char ** argv)'
    except KeyError:
        pass

    ret = context.TryLink("""
        #include <vorbis/vorbisfile.h>
        #include <stdio.h>

        %(main)s {
          OggVorbis_File ovf;
          FILE * f;
          ov_open_callbacks(f, &ovf, 0, 0, OV_CALLBACKS_DEFAULT);
          return 0;
        }
    """ % {'main' : main}, ".c")

    if not ret:
        context.sconf.env = tmp

    context.Result(utils.colorResult(ret))
    return ret

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
    environment['LINKCOM'] = '$CXX $LINKFLAGS $SOURCES $_FRAMEWORKS -Wl,-all_load $ARCHIVES $_LIBDIRFLAGS $_LIBFLAGS -o $TARGET'

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
                    "CheckOgg" : checkStaticOgg,
                    "CheckAllegro4" : checkStaticAllegro4,
                    "CheckAllegro5" : checkStaticAllegro5}

    config = environment.Configure(custom_tests = custom_tests)
    config.CheckOgg()
    environment = config.Finish()

    return utils.configure_backend(environment, backends, custom_tests)
