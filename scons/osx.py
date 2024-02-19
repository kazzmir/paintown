import os
import utils
from SCons.Environment import Environment
from SCons.Script import Exit
from SCons.Script import Action
from SCons.Script import Split

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
        # FIXME: move sdl main to a new check
        env.Append(CPPDEFINES = ['USE_SDL_MAIN'])
    except Exception:
        context.Result(utils.colorResult(0))
        return 0
    context.Result(utils.colorResult(1))
    return 1

def checkStaticOgg(context):
    context.Message("Checking for static ogg and vorbis... ")
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

def checkStaticZ(context):
    context.Message("Checking for static z... ")
    tmp = context.env.Clone()
    env = context.env
    (ok, stuff) = context.TryAction(Action("pkg-config --version"))
    if ok:
        try:
            utils.safeParseConfig(env, 'pkg-config zlib --cflags')
            # Strip off the -L part
            libdir = utils.readExec('pkg-config zlib --libs-only-L')[2:].rstrip()
            # Hack to hardcode these libraries
            zlib = env.Install('misc', "%s/libz.a" % libdir)
            env.Append(LIBS = [zlib])
        except OSError:
            context.sconf.env = tmp
            context.Result(utils.colorResult(0))
            return 0
   
    # FIXME: write proper test
    ret = context.TryLink("""
        int main(int argc, char ** argv){
          return 0;
        }
    """, ".c")

    if not ret:
        context.sconf.env = tmp

    context.Result(utils.colorResult(ret))
    return ret

def findLibDir(output):
    import re
    find = re.compile('-L([^\\s]*)')
    match = find.match(output)
    if match:
        return match.group(1)
    raise Exception("Could not find lib dir!")

def checkStaticFreetype(context):
    context.Message("Checking for static freetype... ")
    tmp = context.env.Clone()
    env = context.env
    (ok, stuff) = context.TryAction(Action("pkg-config --version"))
    if ok:
        try:
            utils.safeParseConfig(env, 'freetype-config --cflags')
            # Strip off the -L part
            libdir = findLibDir(utils.readExec('freetype-config --libs'))
            # Hack to hardcode these libraries
            freetype = env.Install('misc', '%s/libfreetype.a' % libdir)
            env.Append(LIBS = [freetype])
        except OSError:
            context.sconf.env = tmp
            context.Result(utils.colorResult(0))
            return 0
        except Exception, e:
            print e
            context.sconf.env = tmp
            context.Result(utils.colorResult(0))
            return 0

    # FIXME: write proper test
    ret = context.TryLink("""
        #include <stdio.h>

        int main(int argc, char ** argv){
          return 0;
        }
    """, ".c")

    if not ret:
        context.sconf.env = tmp

    context.Result(utils.colorResult(ret))
    return ret

def checkStaticPng(context):
    context.Message("Checking for static png... ")
    tmp = context.env.Clone()
    env = context.env
    try:
        utils.safeParseConfig(env, 'libpng-config --cflags')
        libdir = utils.readExec('libpng-config --libdir')
        # Hack to hardcode these libraries
        png = env.Install('misc', '%s/libpng.a' % libdir)
        env.Append(LIBS = [png])
    except OSError:
        context.sconf.env = tmp
        context.Result(utils.colorResult(0))
        return 0
    except Exception, e:
        print e
        context.sconf.env = tmp
        context.Result(utils.colorResult(0))
        return 0

    # FIXME: write proper test
    ret = context.TryLink("""
        #include <stdio.h>

        int main(int argc, char ** argv){
          return 0;
        }
    """, ".c")

    if not ret:
        context.sconf.env = tmp

    context.Result(utils.colorResult(ret))
    return ret

def checkMallocH(context):
    tmp = context.env.Clone()
    env = context.env

    env.Append(CPPPATH = ['/usr/include/sys'])
    ok = context.sconf.CheckCHeader('malloc.h')
    if not ok:
        context.sconf.env = tmp
    context.Result(ok)
    return ok
 
def checkStaticMpg123(context):
    context.Message("Checking for static libmpg123... ")
    tmp = context.env.Clone()
    env = context.env
    env['HAVE_MP3_MPG123'] = True
    env.Append(CPPDEFINES = ['HAVE_MP3_MPG123'])
    (ok, stuff) = context.TryAction(Action("pkg-config --version"))
    if ok:
        try:
            utils.safeParseConfig(env,'pkg-config libmpg123 --cflags') 
            # Strip off the -L part
            libdir = utils.readExec('pkg-config libmpg123 --libs-only-L')[2:].rstrip()
            # Hack to hardcode these libraries
            mpg123 = env.Install('misc', "%s/libmpg123.a" % libdir)
            env.Append(LIBS = [mpg123])
        except OSError:
            context.sconf.env = tmp
            context.Result(utils.colorResult(0))
            return 0
            
    ret = context.TryLink("""
        #include <mpg123.h>
        int main(int argc, char ** argv){
          int err = mpg123_init();
          if (err == MPG123_OK){
            return 0;
          } 
          return 1;
        }
    """, ".c")

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
    environment['PAINTOWN_NETWORKING'] = True
    environment.Append(CPPDEFINES = ['HAVE_NETWORKING'])
    environment.Append(CPPDEFINES = ['MACOSX'])
    environment['LIBS'] = []
    environment['PEG_MAKE'] = "%s %s" % (utils.colorize('Creating peg parser', peg_color), utils.colorize('$TARGET', 'light-blue'))
    environment.Append(BUILDERS = {'Peg' : utils.pegBuilder(environment)})
    environment.Append(CPPPATH = ['#src', '#src/util/network/hawknl'])
    # environment.Append(CCFLAGS = Split("-arch i386 -arch x86_64"))
    # print environment['CCCOM']
    # I don't know why appending -arch to ccflags doesn't work, but whatever
    environment['CCCOM'] = '$CC -arch i386 -arch x86_64 $CFLAGS $CCFLAGS $_CCCOMCOM $SOURCES -c -o $TARGET'
    environment['CXXCOM'] = '$CXX -arch i386 -arch x86_64 -o $TARGET -c $CXXFLAGS $CCFLAGS $_CCCOMCOM $SOURCES'
    environment['LINKCOM'] = '$CXX $LINKFLAGS -arch i386 -arch x86_64 $SOURCES $_FRAMEWORKS -Wl,-all_load $ARCHIVES $_LIBDIRFLAGS $_LIBFLAGS -o $TARGET'

    # Default preference for a graphics renderer / input system
    backends = ['SDL', 'Allegro4', 'Allegro5']

    if utils.useAllegro4():
        backends = ['Allegro4', 'SDL', 'Allegro5']

    if utils.useAllegro5():
        backends = ['Allegro5', 'SDL', 'Allegro4']

    #environment.ParseConfig('freetype-config --libs --cflags')
    #environment.ParseConfig('libpng-config --libs --cflags')

    if utils.useLLVM():
        llvm(environment)

    custom_tests = {"CheckCompiler": utils.checkCompiler,
                    "CheckSDL" : checkStaticSDL,
                    "CheckOgg" : checkStaticOgg,
                    "CheckFreetype" : checkStaticFreetype,
                    "CheckMalloc" : checkMallocH,
                    "CheckZ" : checkStaticZ,
                    "CheckPng" : checkStaticPng,
                    "CheckMpg123" : checkStaticMpg123,
                    "CheckAllegro4" : checkStaticAllegro4,
                    "CheckAllegro5" : checkStaticAllegro5}

    config = environment.Configure(custom_tests = custom_tests)
    config.CheckZ()
    config.CheckOgg()
    config.CheckMpg123()
    config.CheckFreetype()
    config.CheckPng()
    # config.CheckMalloc()
    environment = config.Finish()

    return utils.configure_backend(environment, backends, custom_tests)
