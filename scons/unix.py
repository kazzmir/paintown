# SCons build file for Linux

import os
import utils
from SCons.Environment import Environment
from SCons.Script import Exit

def checkPython(context):
    import distutils.sysconfig
    import utils
    context.Message("Checking if python is embeddable... ")

    include_path = distutils.sysconfig.get_config_var('INCLUDEPY')
    link_stuff = distutils.sysconfig.get_config_var('LINKFORSHARED')
    # libs = distutils.sysconfig.get_config_var('LDLIBRARY')
    libs = distutils.sysconfig.get_config_var('LIBRARY')
    lib_path = distutils.sysconfig.get_config_var('LIBP')

    # hacks for windows because distutils is broken
    if libs == None and isWindows():
        libs = ['python26']
    if lib_path == None and isWindows():
        import os
        lib_path = os.path.join(os.path.dirname(include_path),'libs')

    # hacks for osx because distutils doesn't quote things
    if utils.isOSX() or utils.isOSX104():
        import re
        f = re.compile('(-framework System Python.framework/Versions/.*/Python)')
        link_stuff = re.sub(f, r"'\1'", link_stuff)

    tmp = context.env.Clone()
    env = context.env
    if include_path != None:
        env.Append(CPPPATH = [include_path])
    if link_stuff != None:
        env.Append(LINKFLAGS = link_stuff.split(' '))
    if lib_path != None:
        env.Append(LIBPATH = [lib_path])
    new_libs = []
    if libs != None:
        new_libs = libs
    old_libs = env['LIBS']
    env.Replace(LIBS = new_libs)
    env.Append(CPPDEFINES = ['HAVE_PYTHON'])
    ret = context.TryLink("""
        #include <Python.h>
        int main(int argc, char *argv[]) {
            Py_Initialize();
            return 0;
        }
    """, ".c");

    env.Append(LIBS = old_libs)

    if not ret:
        context.sconf.env = tmp

    context.Result(utils.colorResult(ret))
    return ret


def checkCompiler(context):
    context.Message("Checking for a compiler (%s) ... " % context.env['CXX'])
    ok = context.TryCompile("""
      int main(int argc, char ** argv){
        return 0;
      }
    """, ".cpp")
    context.Result(utils.colorResult(ok))
    return ok

def checkAllegro5(context):
    context.Message("Checking for Allegro 5 ... ")
    tmp = context.env.Clone()
    env = context.env
    def find(version):
        context.Message(str(version))
        try:
            def make(name):
                return '%s-%s' % (name, version)
            libraries = [make('allegro'),
                         make('allegro_ttf'),
                         make('allegro_memfile'),
                         make('allegro_image'),
                         make('allegro_primitives'),
                         make('allegro_audio'),
                         make('allegro_acodec')]
            utils.safeParseConfig(env, 'pkg-config %s --cflags --libs' % ' '.join(libraries))
            env.Append(CPPDEFINES = ['USE_ALLEGRO5'])
            context.Message('found version %s' % version)
            return True
        except Exception:
            return False
    try:
        ok = 0
        # if find(5.1) or find(5.0):
        if find(5.1):
            ok = 1
        context.Result(utils.colorResult(ok))
        return ok
    except:
        context.sconf.env = tmp
    context.Result(utils.colorResult(0))
    return 0

def checkAllegro4(context):
    context.Message("Checking for Allegro4... ")

    def testAllegro(context):
        return context.TryLink("""
            #include <allegro.h>
            int main(int argc, char ** argv){
              install_allegro(0, NULL, NULL);
              return 0;
            }
            END_OF_MAIN()
        """, ".c")

    # use pkg-config
    def allegro44(context):
        tmp = context.env.Clone()
        env = context.env
        ok = 1
        try:
            utils.safeParseConfig(env, 'pkg-config allegro --cflags --libs')
            env.Append(CPPDEFINES = ['USE_ALLEGRO'])
            ok = testAllegro(context)
        except OSError:
            ok = 0 

        if not ok:
            context.sconf.env = tmp
        else:
            context.Message('found 4.4')

        return ok

    # use allegro-config
    def allegro42(context):
        tmp = context.env.Clone()
        env = context.env
        ok = 1
        try:
            def enableAllegro(env2):
                utils.safeParseConfig(env2, 'allegro-config --cflags --libs')
                env2.Append(CPPDEFINES = ['USE_ALLEGRO'])
            utils.safeParseConfig(env, 'allegro-config --cflags --libs')
            env['paintown_enableAllegro'] = enableAllegro
            env.Append(CPPDEFINES = ['USE_ALLEGRO'])
            ok = testAllegro(context)
        except OSError:
            ok = 0 

        if not ok:
            context.sconf.env = tmp
        else:
            context.Message('found 4.2')

        return ok

    ok = allegro44(context) or allegro42(context)

    context.Result(utils.colorResult(ok))
    return ok

def checkSDL(context):
    context.Message("Checking for SDL ... ")

    def build(x):
        return context.TryLink("""
        #include <SDL.h>
        int main(int argc, char ** argv){
          int %sok = SDL_INIT_VIDEO;
          return SDL_Init(0);
        }
    """ % x, ".c")

    def tryNormal():
        tmp = context.env.Clone()
        env = context.env
        try:
            utils.safeParseConfig(env, 'sdl-config --cflags --libs')
            env.Append(CPPDEFINES = ['USE_SDL'])
            if build('a'):
                return True
            else:
                raise Exception()
        except Exception:
            context.sconf.env = tmp
            return False

    # Put any system libraries after SDL
    def tryMoveLibs():
        tmp = context.env.Clone()
        env = context.env
        try:
            libs = []
            try:
                libs = env['LIBS']
            except KeyError:
                pass
            env.Replace(LIBS = [])
            utils.safeParseConfig(env, 'sdl-config --cflags --libs')
            env.Append(LIBS = libs)
            env.Append(CPPDEFINES = ['USE_SDL'])
            m = build('b')
            if m:
                return True
            else:
                raise Exception("Couldn't build it")
        except Exception, e:
            # print "Moving libraries failed! because '%s'" % e
            context.sconf.env = tmp
            return False

    def tryFramework():
        tmp = context.env.Clone()
        env = context.env
        env.Append(FRAMEWORKS = ['SDL', 'Cocoa'])
        env.Append(CPPDEFINES = ['USE_SDL'])
        env.Append(CPPPATH = ['/Library/Frameworks/SDL.framework/Headers',
                              '/System/Library/Frameworks/Foundation.framework/Headers'])
        main = env.StaticLibrary('src/util/sdl/SDLMain.m')
        env.Append(LIBS = [main])
        m = build('c')
        if m:
            return True
        else:
            context.sconf.env = tmp
            return False

    ok = int(tryNormal() or tryMoveLibs() or tryFramework())
    context.Result(utils.colorResult(ok))
    return ok

def configure(environment, backends):
    custom_tests = {"CheckCompiler": checkCompiler,
                    "CheckSDL" : checkSDL,
                    "CheckAllegro4" : checkAllegro4,
                    "CheckAllegro5" : checkAllegro5}
    config = environment.Configure(custom_tests = custom_tests)

    if not config.CheckCompiler():
        print("No c++ compiler found. Install gcc or clang")
        Exit(1)

    class OkBackend(Exception):
        pass

    try:
        for backend in backends:
            if backend == 'SDL' and config.CheckSDL():
                environment.Append(CPPDEFINES = ['USE_SDL'])
                environment['PAINTOWN_BACKEND'] = 'sdl'
                environment.Append(PAINTOWN_PLATFORM = ['sdl'])
                raise OkBackend()
            if backend == 'Allegro4' and config.CheckAllegro4():
                environment.Append(CPPDEFINES = ['USE_ALLEGRO'])
                environment['PAINTOWN_BACKEND'] = 'allegro4'
                environment.Append(PAINTOWN_PLATFORM = ['allegro4'])
                raise OkBackend()
            if backend == 'Allegro5' and config.CheckAllegro5():
                environment.Append(CPPDEFINES = ['USE_ALLEGRO5'])
                environment['PAINTOWN_BACKEND'] = 'allegro5'
                environment.Append(PAINTOWN_PLATFORM = ['allegro5'])
                raise OkBackend()
    except OkBackend:
        pass

    return config.Finish()

def llvm(environment):
    environment['CXX'] = 'clang++'
    environment['CC'] = 'clang'

def getEnvironment():
    import utils
    environment = Environment(ENV = os.environ)
    peg_color = 'light-cyan'
    environment['PAINTOWN_PLATFORM'] = ['unix']
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
    return configure(environment, backends)
