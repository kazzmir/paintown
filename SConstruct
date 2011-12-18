import os

def isQuiet():
    import sys
    return '-s' in sys.argv

def isPlatform(platform):
    import sys
    return platform in sys.platform

def isWindows():
    return isPlatform("win32")

def isOSX104():
    import platform
    return isPlatform("darwin") and platform.processor() == 'powerpc'

# Assume 10.6 and up
def isOSX():
    return isPlatform("darwin") and not isOSX104()

def isLinux():
    return isPlatform("linux")

def file_exists(path):
    return os.path.exists(path)

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

def safeParseConfig(environment, config):
    # redirects stderr, not super safe
    def version1():
        out = open('fail.log', 'w')
        old_stderr = sys.stderr
        try:
            sys.stderr = out
            environment.ParseConfig(config)
            out.close()
            sys.stderr = old_stderr
        except Exception, e:
            out.close()
            sys.stderr = old_stderr
            raise e
    # use the subprocess module to pass the output of stdout directly
    # to mergeflags and trash stderr
    # Not done yet!! This requires python 2.4
    def version2():
        import subprocess
        process = subprocess.Popen(config.split(' '), stdout = subprocess.PIPE)
        # p = subprocess.Popen(["ruby", "-e", code], stdout = subprocess.PIPE, stderr = subprocess.PIPE)
        out = p.stdout.readline().strip()
        environment.MergeFlags(out)

    version1()

def makeUseEnvironment(key, default):
    def use():
        import os
        try:
            return int(os.environ[key]) == 1
        except KeyError:
            return default
    return use

def makeUseArgument(key, default):
    def use():
        try:
            return int(ARGUMENTS[key]) == 1
        except KeyError:
            return default
    return use

# Functions made with `makeUseArgument' should be set by command line arguments
# to scons:
# $ scons gch=1
# If `makeUseEnvironment' is used instead then a shell variable should be set:
# $ export prx=1
# $ scons
# 12/18/2011 - disable gch for now because multiple environments try to
# build the same files:
# scons: warning: Two different environments were specified for target /home/jon/svn/paintown/build/sdl-debug/util/system.h.gch,
useGch = makeUseArgument('gch', False)
usePrx = makeUseEnvironment('prx', False)
isVerbose = makeUseArgument('verbose', False)
useIntel = makeUseEnvironment('intel', False)
useMinpspw = makeUseEnvironment('minpspw', False)
useAndroid = makeUseEnvironment('android', False)
useIos = makeUseEnvironment('ios', False)
usePs3 = makeUseEnvironment('ps3', False)
useNDS = makeUseEnvironment('nds', False)
useDingoo = makeUseEnvironment('dingoo', False)
usePandora = makeUseEnvironment('pandora', False)
useWii = makeUseEnvironment('wii', False)
useLLVM = makeUseEnvironment('llvm', False)
useNacl = makeUseEnvironment('nacl', False)
nativeCompile = makeUseEnvironment('native', False)
enableProfiled = makeUseEnvironment('PROFILE', False)
showTiming = makeUseEnvironment('timing', False)

def ps3devPath():
    try:
        return os.environ['PS3DEV']
    except KeyError:
        return '/opt/ps3dev'

rtti_counter = 0
def checkRTTI(context):
    global rtti_counter
    rtti_counter += 1
    context.Message("Checking if we need rtti... ")
    tmp = context.env.Clone()
    env = context.env
    env.Append(CXXFLAGS = ['-fno-rtti'])

    ret = context.TryCompile("""
      #include <exception>
      int main(int argc, char ** argv){
        extern void foo();
        try{
          foo();
        } catch (const std::exception & e){
          return 1;
        }
        return 0;
      }
    """, ".cpp")

    s1 = context.lastTarget

    ret = context.TryCompile("""
      #include <exception>
      void foo(){
        throw std::exception();
      }
    """, ".cpp")

    s2 = context.lastTarget

    result = None
    spawn = context.sconf.env['SPAWN']
    try:
        context.sconf.env['SPAWN'] = context.sconf.pspawn_wrapper
        nodes = env.Program(context.sconf.confdir.File('rtti%d' % rtti_counter), [s1,s2])
        result = context.sconf.BuildNodes(nodes)
    except Exception:
        result = False

    context.sconf.env['SPAWN'] = spawn

    foo = 0

    if not result:
        context.sconf.env = tmp
        foo = 1

    context.Result(colorResult(foo))
    return foo

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
            safeParseConfig(env, 'pkg-config %s --cflags --libs' % ' '.join(libraries))
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
        context.Result(colorResult(ok))
        return ok
    except:
        context.sconf.env = tmp
    context.Result(colorResult(0))
    return 0

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
            safeParseConfig(env, 'sdl-config --cflags --libs')
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
            safeParseConfig(env, 'sdl-config --cflags --libs')
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
    context.Result(colorResult(ok))
    return ok

def checkSDLMain(context):
    context.Message("Checking for SDL main... ")
    tmp = context.env.Clone()
    env = context.env

    ok = context.TryLink("""
#include <SDL.h>
int SDL_main(int argc, char ** argv){
    return 0;
}
""", ".c")
    if not ok:
        context.sconf.env = tmp
    else:
        env.Append(CPPDEFINES = ['USE_SDL_MAIN'])
    
    context.Result(colorResult(ok))
    return ok

def checkStaticSDL(context):
    context.Message("Checking for static SDL... ")
    env = context.env

    try:
        safeParseConfig(env, 'sdl-config --static-libs --cflags')
        env.Append(CPPDEFINES = ['USE_SDL'])
    except Exception:
        context.Result(colorResult(0))
        return 0

    if False:
        sdl = env.Install('misc', readExec('sdl-config --prefix') + '/lib/libSDL.a')
        env.Append(LIBS = [sdl])
        safeParseConfig(env, 'sdl-config --cflags')
        env.Append(CPPDEFINES = ['USE_SDL'])
        if isOSX() or isOSX104():
            def framework(x):
                return "-framework %s" % x
            frameworks = Split("""
Cocoa
Carbon
IOKit
System
CoreAudio
AudioUnit
AudioToolbox
QuickTime
OpenGL
""")
            # env.Append(LINKFLAGS = map(framework, frameworks))
            env.Append(FRAMEWORKS = frameworks)
    context.Result(colorResult(1))
    return 1

def checkAllegro(context):
    context.Message("Checking for Allegro... ")

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
            safeParseConfig(env, 'pkg-config allegro --cflags --libs')
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
                safeParseConfig(env2, 'allegro-config --cflags --libs')
                env2.Append(CPPDEFINES = ['USE_ALLEGRO'])
            safeParseConfig(env, 'allegro-config --cflags --libs')
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

    context.Result(colorResult(ok))
    return ok

def checkPthreads(context):
    context.Message("Checking for threads... ")
    if useAllegro():
        env = context.env
        env.Append(LIBS = ['pthread'])
        context.Message(" pthreads")
        context.Result(colorResult(1))
        return 1
    if useAllegro5():
        env = context.env
        env.Append(LIBS = ['pthread'])
        context.Message(' pthreads')
        context.Result(colorResult(1))
        return 1
    if useSDL():
        context.Message(" SDL threads")
        context.Result(colorResult(1))
        return 1
    context.Message(" defaulting to pthreads")
    context.Result(colorResult(1))
    return 1
        #if not useWii() and not useMinpspw():
        #    env.Append(LIBS = [ 'pthread' ])

def checkNativeOgg(context):
    context.Message("Checking for ogg and vorbis... ")
    tmp = context.env.Clone()
    env = context.env
    env['HAVE_OGG'] = True
    env.Append(CPPDEFINES = ['HAVE_OGG'])
    (ok, stuff) = context.TryAction(Action("pkg-config --version"))
    if ok:
        try:
            safeParseConfig(env, 'pkg-config vorbisfile --libs --cflags')
        except OSError:
            context.sconf.env = tmp
            context.Result(colorResult(0))
            return 0

    ret = context.TryLink("""
        #include <vorbis/vorbisfile.h>
        #include <stdio.h>

        /* nacl stuff. remove this once nacl gets its own scons file */
        int __wrap_open(const char * path, int mode, int params){
            return 0;
        }

        int __wrap_read(int fd, void * buf, int count){
            return 0;
        }

        int __wrap_close(int fd){
            return 0;
        }

        int __wrap_lseek(int fd, int offset, int whence){
            return 0;
        }

        int main(int argc, char ** argv){
          OggVorbis_File ovf;
          FILE * f;
          ov_open_callbacks(f, &ovf, 0, 0, OV_CALLBACKS_DEFAULT);
          return 0;
        }
    """, ".c")

    if not ret:
        context.sconf.env = tmp

    context.Result(colorResult(ret))
    return ret
    
def checkMpg123(context):
    context.Message("Checking for libmpg123... ")
    tmp = context.env.Clone()
    env = context.env
    env['HAVE_MP3_MPG123'] = True
    env.Append(CPPDEFINES = ['HAVE_MP3_MPG123'])
    (ok, stuff) = context.TryAction(Action("pkg-config --version"))
    if ok:
        try:
            safeParseConfig(env,'pkg-config libmpg123 --libs --cflags') 
        except OSError:
            context.sconf.env = tmp
            context.Result(colorResult(0))
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

    context.Result(colorResult(ret))
    return ret

# Alternatively use libmad if mpg123 is not available
def checkMad(context):
    context.Message("Checking for libmad... ")
    tmp = context.env.Clone()
    env = context.env
    env['HAVE_MP3_MAD'] = True
    env.Append(CPPDEFINES = ['HAVE_MP3_MAD'])
    def tryPkgConfig():
        (ok, stuff) = context.TryAction(Action("pkg-config --version"))
        if ok:
            try:
                safeParseConfig(env, 'pkg-config mad --libs --cflags') 
                return True
            except OSError:
                # context.sconf.env = tmp
                # context.Result(colorResult(0))
                return False
        return False

    def tryLib():
        env.Append(LIBS = ['mad'])

    tryPkgConfig() or tryLib()
                
    ret = context.TryLink("""
        #include <mad.h>
        int main(int argc, char ** argv){
          struct mad_stream stream;
          mad_stream_init(&stream);
          return 0;
        }
    """, ".c")

    if not ret:
        context.sconf.env = tmp

    context.Result(colorResult(ret))
    return ret

def checkPython(context):
    import distutils.sysconfig
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
    if isOSX() or isOSX104():
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

    context.Result(colorResult(ret))
    return ret

def rubyConfigVariable(var):
    # gets the ruby configuration stuff and expands config variables
    import subprocess
    code = """
require 'mkmf'

def replace(str)
    str.gsub(/\$\(\w+\)/){|x| replace(CONFIG[x[/\w+/]]) }
end     

puts replace(CONFIG['%s'])
    """ % var
    try:
        p = subprocess.Popen(["ruby", "-e", code], stdout = subprocess.PIPE, stderr = subprocess.PIPE)
        return p.stdout.readline().strip()
    except Exception:
        return ""

def rubyDir():
    return rubyConfigVariable('archdir')

def rubyLib():
    return rubyConfigVariable('RUBY_SO_NAME')

def rubyStaticLib():
    return rubyConfigVariable('LIBRUBY_A')

def checkRuby(context):
    context.Message("Checking if ruby is embeddable... ")
    if not canRunRuby(context):
        context.Result(colorResult(0))
        return 0
    tmp = context.env.Clone()
    env = context.env
    env.Append(CPPDEFINES = ['HAVE_RUBY'])
    env.Append(CPPPATH = [rubyDir()])
    old_libs = env['LIBS']
    env.Replace(LIBS = [rubyLib()])

    ret = context.TryLink("""
        #include <ruby.h>
        int main(int argc, char ** argv){
            ruby_init();
            return 0;
        }
    """, ".c")

    env.Replace(LIBS = old_libs + [rubyLib()])

    if not ret:
        context.sconf.env = tmp

    context.Result(colorResult(ret))
    return ret

def checkStaticRuby(context):
    context.Message("Checking if ruby is statically embeddable... ")
    if not canRunRuby(context):
        context.Result(colorResult(0))
        return 0

    tmp = context.env.Clone()
    env = context.env
    env.Append(CPPDEFINES = ['HAVE_RUBY'])
    env.Append(CPPPATH = [rubyDir()])
    old_libs = env['LIBS']
    env.Replace(LIBS = [rubyStaticLib(), 'crypt', 'pthread', 'm', 'dl'])

    ret = context.TryLink("""
        #include <ruby.h>
        int main(int argc, char ** argv){
            ruby_init();
            return 0;
        }
    """, ".c")
    
    env.Replace(LIBS = old_libs + [rubyLib()])

    if not ret:
        context.sconf.env = tmp

    context.Result(colorResult(ret))
    return ret

def canRunRuby(context):
    (ok, stuff) = context.TryAction(Action("ruby -v"))
    return ok == 1

def checkRunRuby(context):
    # just fail for now
    context.Result(colorResult(0))
    return 0
    context.Message("Checking if we can run ruby... ")
    (ok, stuff) = context.TryAction(Action("ruby -v"))
    context.Result(colorResult(ok))
    return ok

# find freetype in windows since we dont have freetype-config
def checkWindowsFreeType(context):
    context.Message("Checking for Freetype... ")
    def build():
        return context.TryCompile("""
#include <ft2build.h>
#include FT_FREETYPE_H
int main(int argc, char ** argv){
    int major = FREETYPE_MAJOR; 
    return 0;
}
""", ".c")
    if not build():
        if useMingw():
            import os
            tmp = context.env.Clone()
            mingw = os.environ['MINGDIR']
            context.env.Append(CPPPATH = ["%s/include/freetype2" % mingw])
            if not build():
                context.env = tmp
                context.Result(colorResult(0))
                return 0
        else:
            context.Message("don't know how to find freetype for a non-mingw compiler")
            context.Result(colorResult(0))
            return 0

    context.Result(colorResult(1))
    return 1

def useAllegro5():
    def byEnv():
        try:
            return os.environ['ALLEGRO5'] == '1'
        except KeyError:
            return False

    def byArgument():
        try:
            return int(ARGUMENTS['allegro5']) == 1
        except KeyError:
            return False

    return byEnv() or byArgument()

def useAllegro():
    def byEnv():
        try:
            return os.environ['ALLEGRO'] == '1'
        except KeyError:
            return False

    def byArgument():
        try:
            return int(ARGUMENTS['allegro']) == 1
        except KeyError:
            return False
        
    return byEnv() or byArgument()

def useSDL():
    return not useAllegro() and not useAllegro5()

def isCygwin():
    try:
        return os.environ['CYGWIN'] == '1'
    except:
        return False    

def useMingw():
    try:
        return "mingw" in ARGUMENTS[ 'env' ]
    except KeyError:
        return False

def readExec( program ):
    try:
        return os.popen(program).readline().replace("\n",'')
    except OSError:
        return ""

def getDebug():
    try:
        return int(os.environ[ 'DEBUG' ])
    except KeyError:
        return 0

def less_verbose(env):
    link_color = 'light-red'
    ar_color = 'yellow'
    ranlib_color = 'light-purple'
    peg_color = 'light-cyan'
    env['CCCOMSTR'] = "%s %s" % (colorize('Compiling c file', 'light-green'), colorize('$SOURCE', 'light-blue'))
    env['SHCCCOMSTR'] = "%s %s" % (colorize('Compiling c file', 'light-green'), colorize('$SOURCE', 'light-blue'))
    env['CXXCOMSTR'] = "%s %s" % (colorize('Compiling c++ file', 'light-green'), colorize('$SOURCE', 'light-blue'))
    env['SHCXXCOMSTR'] = "%s %s" % (colorize('Compiling c++ file', 'light-green'), colorize('$SOURCE', 'light-blue'))
    env['LINKCOMSTR'] = "%s %s" % (colorize('Linking', link_color), colorize('$TARGET', 'light-blue'))
    env['SHLINKCOMSTR'] = "%s %s" % (colorize('Linking', link_color), colorize('$TARGET', 'light-blue'))
    env['ARCOMSTR'] = "%s %s" % (colorize('Building library', ar_color), colorize('$TARGET', 'light-blue'))
    env['RANLIBCOMSTR'] = "%s %s" % (colorize('Indexing library', ranlib_color), colorize('$TARGET', 'light-blue'))
    env['PEG_MAKE'] = "%s %s" % (colorize('Creating peg parser', peg_color), colorize('$TARGET', 'light-blue'))
    return env

def getEnvironment(debug):
    def intel(env):
        env['CC'] = 'icc'
        env['CXX'] = 'icpc'
        # '-Werror-all', '-Weffc++'
        # TODO: replace these flags with descriptive names
        env.Append(CCFLAGS = ['-wd981', '-wd2259'],
                   CXXFLAGS = ['-wd981', '-wd271',
                               '-wd383', '-wd869',
                               '-wd1599'])
        return env

    def dingux(env):
        import os
        print "Environment is Dingux"
        mips = '/opt/mipsel-linux-uclibc'
        bin_path = mips + '/usr/bin'
        prefix = 'mipsel-linux-'
        def setup(pre, x):
            return '%s%s' % (pre, x)
        env['CC'] = setup(prefix, 'gcc')
        env['LD'] = setup(prefix, 'ld')
        env['CXX'] = setup(prefix, 'g++')
        env['AS'] = setup(prefix, 'as')
        env['AR'] = setup(prefix, 'ar')
        env['OBJCOPY'] = setup(prefix, 'objcopy')
        env.Append(CPPDEFINES = ['DINGOO'])
        # env.Append(CPPDEFINES = ['DINGOO', 'LINUX', 'MPU_JZ4740'])
        #env.Append(CPPPATH = [setup(dingoo_path, "/include"),
        #                      setup(dingoo_path, "/include/SDL"),
        #                      setup(dingoo_path, "/include/freetype2")])
        #env.Append(LIBPATH = [setup(dingoo_path, '/lib')])
        #flags = ['-G0', '-mips32', '-mno-abicalls', '-msoft-float', '-fno-builtin', '-fno-pic']
        #env.Append(CCFLAGS = flags)
        #env.Append(CXXFLAGS = flags)
        #env.Append(LINKFLAGS = ['-nodefaultlibs', '%s/lib/dingoo.xn' % dingoo_path])
        # env.Append(LIBS = ['c', 'm', 'fgl', 'sml', 'jz4740', 'gcc'])
        #env.Append(LIBS = ['SDL', 'jz4740', 'sml', 'fgl', 'c', 'gcc'])
        #env['LINKCOM'] = '$LD $LINKFLAGS $SOURCES $_LIBDIRFLAGS $_LIBFLAGS -o $TARGET'
        env.PrependENVPath('PATH', bin_path)
        # env.PrependENVPath('PATH', '%s/bin' % dingoo_path)
        return env

    def dingoo(env):
        import os
        print "Environment is Dingoo"
        dingoo_path = os.environ['DINGOO_SDK']
        mips_path = os.environ['MIPSTOOLS']
        bin_path = mips_path + '/bin'
        prefix = 'mipsel-linux-'
        def setup(pre, x):
            return '%s%s' % (pre, x)
        env['CC'] = setup(prefix, 'gcc')
        env['LD'] = setup(prefix, 'ld')
        env['CXX'] = setup(prefix, 'g++')
        env['AS'] = setup(prefix, 'as')
        env['AR'] = setup(prefix, 'ar')
        env['OBJCOPY'] = setup(prefix, 'objcopy')
        env.Append(CPPDEFINES = ['DINGOO', 'LINUX', 'MPU_JZ4740'])
        env.Append(CPPPATH = [setup(dingoo_path, "/include"),
                              setup(dingoo_path, "/include/SDL"),
                              setup(dingoo_path, "/include/freetype2")])
        env.Append(LIBPATH = [setup(dingoo_path, '/lib')])
        flags = ['-G0', '-mips32', '-mno-abicalls', '-msoft-float', '-fno-builtin', '-fno-pic']
        env.Append(CCFLAGS = flags)
        env.Append(CXXFLAGS = flags)
        env.Append(LINKFLAGS = ['-nodefaultlibs', '%s/lib/dingoo.xn' % dingoo_path])
        # env.Append(LIBS = ['c', 'm', 'fgl', 'sml', 'jz4740', 'gcc'])
        env.Append(LIBS = ['SDL', 'jz4740', 'sml', 'fgl', 'c', 'gcc'])
        env['LINKCOM'] = '$LD $LINKFLAGS $SOURCES $_LIBDIRFLAGS $_LIBFLAGS -o $TARGET'
        env.PrependENVPath('PATH', bin_path)
        env.PrependENVPath('PATH', '%s/bin' % dingoo_path)
        return env

    def pandora(env):
        import os
        print "Environment is Pandora"
        env['CC'] = os.environ['CC']
        env['CXX'] = os.environ['CXX']
        env['LD'] = os.environ['CC']
        env['AS'] = os.environ['AS']
        env['AR'] = os.environ['AR']
        env.Append(CPPDEFINES = ['PANDORA'])
        flags = ['-O3', '-pipe', '-march=armv7-a', '-mtune=cortex-a8', '-mfpu=neon', '-mfloat-abi=softfp', '-ftree-vectorize', '-ffast-math', '-fsingle-precision-constant']
        env.Append(CCFLAGS = flags)
        env.Append(CXXFLAGS = flags)
        return env

    def nds(env):
        print "Environment is nds"
        path = '/opt/devkitARM'
        bin_path = path + '/bin'
        # libexec_path = path + '/libexec/gcc/arm-eabi/4.5.1'
        prefix = 'arm-eabi-'
        def setup(pre, x):
            return '%s%s' % (pre, x)
        env['CC'] = setup(prefix, 'gcc')
        env['LD'] = setup(prefix, 'ld')
        env['CXX'] = setup(prefix, 'g++')
        env['AS'] = setup(prefix, 'as')
        env['AR'] = setup(prefix, 'ar')
        env['OBJCOPY'] = setup(prefix, 'objcopy')

        env.Append(CPPPATH = [setup(path, "/libnds/include"),
                              setup(path, "/libnds/include/SDL"),
                              setup(path, "/libnds/include/freetype2")])

        env.Append(LIBPATH = [setup(path, '/libnds/lib')])
        env.Append(CPPDEFINES = ['NDS'])
        env.PrependENVPath('PATH', bin_path)
        # env.PrependENVPath('PATH', libexec_path)
        return env

    # minpspw for psp dev environment on windows (and linux?)
    def minpspw(env):
        print "Environment is minpspw (psp development)"
        # on linux, symlink the pspsdk to /opt/pspsdk, or just put it there
        path = '/opt/pspsdk'
        try:
            path = os.environ['MINPSPWDIR']
        except KeyError:
            pass
        bin_path = path + '/bin/'
        prefix = 'psp-'
        def setup(pre, x):
            return '%s%s' % (pre, x)
        env['CC'] = setup(prefix, 'gcc')
        env['LD'] = setup(prefix, 'ld')
        env['CXX'] = setup(prefix, 'g++')
        env['AS'] = setup(prefix, 'as')
        env['AR'] = setup(prefix, 'ar')
        env['OBJCOPY'] = setup(prefix, 'objcopy')
        # FIXME: try to use freetype-config and sdl-config to find these paths
        # instead of hard coding them
        env.Append(CPPPATH = [setup(path, "/psp/include"),
                              setup(path, "/psp/include/SDL"),
                              setup(path, "/psp/include/freetype2"),
                              setup(path, "/psp/sdk/include")])
        env.Append(CPPDEFINES = ['MINPSPW','_PSP_FW_VERSION=150'])
        env.Append(LIBPATH = [setup(path, '/psp/lib'),
                              setup(path, '/psp/sdk/lib')])
        if usePrx():
            env.Append(LINKFLAGS = ['-specs=%s/psp/sdk/lib/prxspecs' % path,
                                    '-Wl,-q,-T%s/psp/sdk/lib/linkfile.prx' % path])
        # So that xmain.cpp can pick up on windows requirement
        if isWindows():
            env.Append(CPPDEFINES = ['INCLUDE_SCE_MODULE_INFO'])
        flags = ['-G0', '-fexceptions']
        env.Append(CCFLAGS = flags)
        env.Append(CXXFLAGS = flags)
        env['LINKCOM'] = '$CC $LINKFLAGS $SOURCES -Wl,--start-group $_LIBDIRFLAGS $_LIBFLAGS -Wl,--end-group -o $TARGET'
        env.Append(LINKFLAGS = flags)
# pthread-psp
        all = Split("""
SDL
SDL_image
SDL_mixer
SDLmain
pspdebug
ogg
vorbis
vorbisfile
GL
stdc++
m
freetype
png
z
jpeg
c
pspctrl
pspvfpu
pspdisplay
psphprm
pspaudio
pspgu
pspge
psprtc
pspsdk
pspuser
pspkernel
pspnet_inet
""")
        env.Append(LIBS = all)
        # os.environ['PATH'] = "%s:%s" % (bin_path, os.environ['PATH'])
        env.PrependENVPath('PATH', bin_path)
        return env
    
     # ps3toolchain with psl1ght dev environment on linux
    def ps3(env):
        print "Environment is ps3 (ps3 development)"
        # symlink the ps3dev to /opt/ps3dev, or just put it there
        # Needs these environment variables
        # export PS3DEV=/opt/ps3dev
        # export PATH=$PATH:$PS3DEV/bin
        # export PATH=$PATH:$PS3DEV/ppu/bin
        # export PATH=$PATH:$PS3DEV/spu/bin
        # export PSL1GHT=$PS3DEV/psl1ght
        # export PATH=$PATH:$PSL1GHT/bin
        path = ps3devPath()
        bin_path = path + '/ppu/bin/'
        prefix = 'ppu-'
        def setup(pre, x):
            return '%s%s' % (pre, x)
        env['CC'] = setup(prefix, 'gcc')
        env['LD'] = setup(prefix, 'ld')
        env['CXX'] = setup(prefix, 'g++')
        env['AS'] = setup(prefix, 'as')
        env['AR'] = setup(prefix, 'ar')
        env['OBJCOPY'] = setup(prefix, 'objcopy')
        # FIXME: try to use sdl-config to find these paths
        # instead of hard coding them
        safeParseConfig(env, path + '/portlibs/ppu/bin/freetype-config --cflags --libs')
        safeParseConfig(env, path + '/portlibs/ppu/bin/libpng-config --cflags --libs')

        # FIXME: it uses -lc-glue-ppu which I can't find maybe I missed something in the setup for now I'll put it down below
        #env.ParseConfig(bin_path +'sdl-config --cflags --libs') 
        
        env.Append(CPPPATH = [setup(path, "/ppu/include"),
                              setup(path, "/spu/include"),
                              setup(path, "/portlibs/ppu/include"),
                              setup(path, "/portlibs/ppu/include/SDL"),
                              setup(path, "/portlibs/ppu/include/vorbis"),
                              setup(path, "/portlibs/ppu/include/ogg"),
                              setup(path, "/psl1ght/ppu/include"),
                              setup(path, "/psl1ght/spu/include")])
        env.Append(CPPDEFINES = ['PS3'])
        env.Append(LIBPATH = [setup(path, '/ppu/lib'),
                              setup(path, '/spu/lib'),
                              setup(path, '/portlibs/ppu/lib'),
                              setup(path, '/psl1ght/ppu/lib'),
                              setup(path, '/psl1ght/spu/lib')])
        # -mminimal-toc puts less stuff in the table of contents "TOC".
        # I don't know why this is important for the ps3 compiler but apparently
        # paintown was overflowing the TOC causing it to crash on startup.
        flags = ['-mcpu=cell', '-mhard-float', '-fmodulo-sched', '-ffunction-sections', '-fdata-sections', '-maltivec', '-mminimal-toc']
        env.Append(CCFLAGS = flags)
        env.Append(CXXFLAGS = flags)
        env['LINKCOM'] = '$CC $LINKFLAGS $SOURCES -Wl,--start-group $_LIBDIRFLAGS $_LIBFLAGS -Wl,--end-group -o $TARGET'
        env.Append(LINKFLAGS = flags)
        # Removed reality and psl1ght
        all = Split("""
SDL
ogg
vorbis
vorbisfile
stdc++
m
gcm_sys
sysutil
lv2
io
net
sysmodule
audio
png
z
jpeg
c
rsx
""")
        env.Append(LIBS = all)
        env.PrependENVPath('PATH', bin_path)
        env.PrependENVPath('PKG_CONFIG_PATH', path + '/portlibs/ppu/lib/pkgconfig')
        return env
        
    # use the devkitpro stuff for wii/gamecube
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
        if isWindows():
            env.Append(CPPDEFINES = ['USE_SDL_MAIN'])
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
        env.Append(CPPPATH = ['#src/wii'])
        # env['LINKCOM'] = '$CC $SOURCES --start-group $_LIBDIRFLAGS $_LIBFLAGS --end-group -o $TARGET'
        env.Append(LIBS = ['wiiuse', 'wiikeyboard', 'iberty', 'bte', 'fat', 'ogc', 'm'])
        # os.environ['PATH'] = "%s:%s:%s" % (bin_path, ogc_bin_path, os.environ['PATH'])
        env.PrependENVPath('PATH', bin_path)
        env.PrependENVPath('PATH', ogc_bin_path)
        return env
    def android(env):
        # Sets up the environment for Google Android
        def setup(pre, x):
            return '%s%s' % (pre, x)
        
        platform = 'android-9'
        path = '/opt/android'
        bin_path = setup(path, '/arm-linux-androideabi-4.4.3/bin')
        prefix = 'arm-linux-androideabi-'
        def set_prefix(x):
            return '%s%s' % (prefix, x)
        env['CC'] = set_prefix('gcc')
        env['LD'] = set_prefix('ld')
        env['CXX'] = set_prefix('g++')
        env['AS'] = set_prefix('as')
        env['AR'] = set_prefix('ar')
        env['OBJCOPY'] = set_prefix('objcopy')
        
        env.Append(CPPPATH = [setup(path, '/arm-linux-androideabi-4.4.3/include'), 
                              setup(path, '/platforms/%s/arch-arm/usr/include' % platform),
                              setup(path, '/platforms/%s/arch-arm/usr/include/SDL' % platform),
                              setup(path, '/platforms/%s/arch-arm/usr/include/freetype' % platform),
                              setup(path, '/sources/cxx-stl/gnu-libstdc++/include')
                             ])
        env.Append(CPPDEFINES = Split("""ANDROID __ARM_ARCH_5__ __ARM_ARCH_5T__ __ARM_ARCH_5E__ __ARM_ARCH_5TE__"""))
        # flags = ['-fpic', '-fexceptions', '-ffunction-sections', '-funwind-tables', '-fstack-protector',  '-Wno-psabi', '-march=armv5te', '-mtune=xscale', '-msoft-float', '-mthumb', '-Os', '-fomit-frame-pointer', '-fno-strict-aliasing', '-finline-limit=64',]
        flags = ['-shared', '-fpic', '-fexceptions', '-ffunction-sections', '-funwind-tables', '-Wno-psabi', '-march=armv5te', '-mtune=xscale', '-msoft-float', '-mthumb', '-Os', '-fomit-frame-pointer', '-fno-strict-aliasing', '-finline-limit=64']
        # linkflags = flags + ['-Wl,--allow-shlib-undefined']
        linkflags = flags
        # libs = ['freetype', 'png', 'SDL', 'm', 'log', 'jnigraphics', 'c', 'm', 'supc++',]
        # Copy the static stdc++ from gnu-libstdc++
        gnustdlib = env.InstallAs('misc/libgnustdc++.a', '/opt/android/sources/cxx-stl/gnu-libstdc++/libs/armeabi/libstdc++.a')
        libs = Split("""freetype2-static png SDL m log c jnigraphics supc++ EGL GLESv2 GLESv1_CM z gnustdc++""")
        env.Append(CCFLAGS = flags)
        env.Append(CXXFLAGS = flags)
        env.Append(LINKFLAGS = linkflags)
        env.Append(CPPPATH = ['#src/android'])
        # Hack to put libstdc++ at the end
        # env['LINKCOM'] = '$CXX $LINKFLAGS $SOURCES $_LIBDIRFLAGS $_LIBFLAGS /opt/android/sources/cxx-stl/gnu-libstdc++/libs/armeabi/libstdc++.a -o $TARGET'
        # env['LINKCOM'] = '$CXX $LINKFLAGS $SOURCES $_LIBDIRFLAGS $_LIBFLAGS -o $TARGET'
        env.Append(LIBS = libs)
        env.Append(LIBPATH = [setup(path, '/platforms/%s/arch-arm/usr/lib' % platform),
                              '#misc'])
        
        env.PrependENVPath('PATH', bin_path)
        return env
    def ios(env):
        # Sets up the environment for Apple IOS
        # FIXME Target correct arm or simulator
        # iPhoneOS.platform
        # iPhoneSimulator.platform

        def setup(pre, x):
            return '%s%s' % (pre, x)
        
        # Check for or target above mentioned platforms
        platform = 'iPhoneOS.platform'
        # Target sdk
        sdk = 'SDKs/iPhoneOS4.3.sdk'
        path = '/Developer/Platforms/%s/Developer' % platform
        bin_path = setup(path, 'usr/bin')
        # Prefix may depend on target platform
        prefix = 'arm-apple-darwin10-'
        def set_prefix(x):
            return '%s%s' % (prefix, x)
        env['CC'] = set_prefix('gcc')
        env['LD'] = set_prefix('ld')
        env['CXX'] = set_prefix('g++')
        env['AS'] = set_prefix('as')
        env['AR'] = set_prefix('ar')
        env['OBJCOPY'] = set_prefix('objcopy')
        
        env.Append(CPPPATH = [setup(path, '/include'), 
                              setup(path, '/%s/usr/include' % sdk)
                             ])
        env.Append(CPPDEFINES = ['IOS'])
        
        flags = ['-shared', '-fpic', '-fexceptions', '-ffunction-sections', '-funwind-tables', '-Wno-psabi', '-march=armv5te', '-mtune=xscale', '-msoft-float', '-mthumb', '-Os', '-fomit-frame-pointer', '-fno-strict-aliasing', '-finline-limit=64']
        
        linkflags = flags
        
        libs = Split("""m c z""")
        env.Append(CCFLAGS = flags)
        env.Append(CXXFLAGS = flags)
        env.Append(LINKFLAGS = linkflags)
        env.Append(CPPPATH = ['#src/ios'])
        
        env.Append(LIBS = libs)
        env.Append(LIBPATH = [setup(path, '%s/usr/lib' % sdk),
                              '#misc'])
        
        env.PrependENVPath('PATH', bin_path)
        return env
    def nacl(env):
        # Sets up the environment for Googles Native Client
        # check for architecture
        print "Environment is for Google Native Client"
        import platform
        arch = platform.architecture()[0]
        def nacl32():
            try:
                return os.environ['nacl_32'] == '1'
            except KeyError:
                return False

        def nacl64():
            try:
                return os.environ['nacl_64'] == '1'
            except KeyError:
                return False

        arch_override = ''
        if nacl32():
            arch_override = '32bit'
            print "Forcing 32bit compile"
        elif nacl64():
            print "Forcing 64bit compile"
            arch_override = '64bit'
        
        def setup(pre, x):
            return '%s%s' % (pre, x)
        
        path = '/opt/nacl_sdk'
        bin_path = setup(path, '/toolchain/linux_x86/bin')
        env.PrependENVPath('PATH', bin_path)
        flags = []
        paths = []
        
        libs = ['srpc', 'ppapi_cpp', 'ppapi', 'ppruntime']

        env['PAINTOWN_NACL_ARCH'] = '32'
        
        if arch == '64bit' and not arch_override == '32bit' or arch_override == '64bit':
            prefix = 'x86_64-nacl-'
            flags += ['-m64']
            libs.append('')
            paths.append([setup(path, '/toolchain/linux_x86/x86_64-nacl/usr/include/')])
            usr_path = setup(path, '/toolchain/linux_x86/x86_64-nacl/usr')
            env['PAINTOWN_NACL_ARCH'] = '64'
        elif arch == '32bit' or arch_override == '32bit':
            prefix = 'nacl-'
            flags += ['-m32']
            libs.append('')
            paths.append([setup(path, '/toolchain/linux_x86/i686-nacl/usr/include')])
            usr_path = setup(path, '/toolchain/linux_x86/i686-nacl/usr')
        
        env.PrependENVPath('PATH', usr_path)
        env.PrependENVPath('PKG_CONFIG_PATH', "%s/%s" % (usr_path, "lib/pkgconfig"))
        paths.append([setup(usr_path,'/include')])
        
        def set_prefix(x):
            return '%s%s' % (prefix, x)
        env['CC'] = set_prefix('gcc')
        env['LD'] = set_prefix('ld')
        env['CXX'] = set_prefix('g++')
        env['AS'] = set_prefix('as')
        env['AR'] = set_prefix('ar')
        env['OBJCOPY'] = set_prefix('objcopy')
        
        safeParseConfig(env, usr_path + '/bin/libpng-config --cflags --libs')
        safeParseConfig(env, usr_path + '/bin/freetype-config --cflags --libs')
        safeParseConfig(env, usr_path + '/bin/sdl-config --cflags --libs')

        compile_flags = ['-fno-builtin', '-fno-stack-protector', '-fdiagnostics-show-option']

        wrapped_symbols = ['open', 'read', 'close', 'lseek', 'lstat', 'access']
        def wrap(symbol):
            return '-Wl,--wrap=%s' % symbol
        env.Append(CPPDEFINES = ['NACL'])
        env.Append(CPPPATH = paths)
        env.Append(CCFLAGS = flags + compile_flags)
        env.Append(CXXFLAGS = flags + compile_flags)
        env.Append(LIBPATH = setup(path, '/toolchain/linux_x86/lib'))
        env.Append(LINKFLAGS = flags + ['-melf_nacl'] + map(wrap, wrapped_symbols))
        env.Append(LIBS = libs)
        return env
    def llvm(env):
        #env['CC'] = 'llvm-gcc'
        #env['CXX'] = 'llvm-g++'
        env['CXX'] = 'clang++'
        env['CC'] = 'clang'

        # <nicholas> ah. don't replace AR or LD or RANLIB.
        # <jonrafkind> dont use llvm-ar and llvm-ld?
        # <nicholas> that's correct.
        # <jonrafkind> ok.. but why do those exist?
        # <nicholas> if you switch CXX to llvm-g++ and change nothing else, it'll work.
        # <jonrafkind> ok
        # <nicholas> well, that's a great question. :)
        # <jonrafkind> heh
        # <nicholas> the llvm-foo tools do what the native tools except to llvm bitcode instead
        #env['AR'] = 'llvm-ar'
        #env['AS'] = 'llvm-as'
        #env['LD'] = 'llvm-ld'
        #env['RANLIB'] = 'llvm-ranlib'
        return env
    def raw():
        defines = []
        if isOSX() or isOSX104():
            defines.append('MACOSX')
            # env.Append(CPPDEFINES = 'MACOSX')
        cflags = []
        if debug:
            defines.append('DEBUG')
            # for gcov:
            # ['-fprofile-arcs', '-ftest-coverage']
            # cflags.append( ['-g3','-ggdb', '-Werror'])
            cflags.extend(['-g3', '-Wfatal-errors'])
        else:
            # -march=native
            if nativeCompile():
                cflags.extend(['-O2', '-g', '-pipe', '-march=native'])
                if not enableProfiled():
                    cflags.extend(['-fomit-frame-pointer'])
            else:
                cflags.extend(['-O2'])

        if isCygwin():
            import SCons.Tool.zip
            env = Environment(ENV = os.environ, tools = ['mingw'])
            env['CXX'] = 'C:\\MinGW\\bin\\g++.exe'
            env['CC'] = 'C:\\MinGW\\bin\\gcc.exe'
            env['AR'] = 'C:\\MinGW\\bin\\ar.exe'
            SCons.Tool.zip.generate(env)
            return env
        elif useMingw():
            return Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags, tools = ['mingw', 'zip'])
        else:
            if useIntel():
                print "Using the intel compiler"
                return intel(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags))
            elif usePandora():
                return pandora(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags))
            elif useNDS():
                return nds(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags))
            elif useDingoo():
                return dingux(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags))
            elif useWii():
                if isWindows():
                    return wii(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags, tools = ['mingw']))
                return wii(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags))
            elif useMinpspw():
                return minpspw(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags, tools = ['mingw']))
            elif useAndroid():
                return android(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags, tools = ['mingw']))
            elif useIos():
                return ios(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags, tools = ['mingw']))
            elif usePs3():
                return ps3(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags, tools = ['mingw']))
            elif useNacl():
                return nacl(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags, tools = ['mingw']))
            elif useLLVM():
                return llvm(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags))
            else:
                return Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags)
    def add_peg(env):
        env['PEG_MAKE'] = 'Creating peg parser $TARGET'
        return env
    if not debug and not isVerbose():
        return less_verbose(raw())
    else:
        return add_peg(raw())
        
if isWindows():
    print "Try 'scons env=mingw' if you want to use mingw's gcc instead of visual studio or borland"
    if not isCygwin():
        print "Cygwin not detected. If are you using cygwin set"
        print "export CYGWIN=1"
    else:
        print "Cygwin detected"

def peg_to_cpp(target, source, env):
    import sys
    sys.path.append("src/mugen/parser")
    sys.path.append(".")
    import peg, re, cpp_generator
    name = source[0].name
    parser = peg.make_peg_parser(re.sub('\..*', '', name))
    fout = open(target[0].path, 'w')
    fout.write(cpp_generator.generate(parser(source[0].path)))
    fout.write('\n')
    fout.close()

def peg_to_cpp_multi(target, source, env):
    import sys
    sys.path.append("src/mugen/parser")
    sys.path.append(".")
    import peg, re, os
    name = re.sub('\..*', '', source[0].name)
    parser = peg.make_peg_parser(name)
    # print "Multi cpp %s" % target
    # print "Target path is %s" % target[0].path
    fout = open(target[0].path, 'w')
    directory = os.path.dirname(target[0].path)
    # fout.write(parser(source[0].path).generate_cpp(separate = name, directory = directory))
    fout.write(parser(source[0].path).generate_cpp(separate = name, directory = directory))
    # fout.write(parser(source[0].path).generate_cpp(separate = name, main = True))
    fout.write('\n')
    fout.close()
    #print "Generated multi"
    #import time
    # time.sleep(10)

def peg_scanner(node, env, path):
    import sys
    #sys.path.append("src/mugen/parser")
    #sys.path.append('../../../src/mugen/parser')
    #sys.path.append(".")
    import peg, re, os
    # print "Node is %s" % node
    name = re.sub('\..*', '', str(os.path.basename(str(node))))
    directory = os.path.dirname(str(node))
    # print "Peg emitter source is '%s' %s" % (source[0].path, source[0].srcnode())
    # print "Peg name is %s" % name
    parser = peg.make_peg_parser(name)
    # print "x1"
    where = '/tmp/scanner.tmp'
    stuff = open(where, 'w')
    stuff.write(node.get_contents())
    stuff.close()
    final = parser(where)
    files = final.list_files(name)
    # print "Generating peg files to %s" % directory
    # final.generate_cpp(separate = name, directory = directory)
    final.generate_cpp(separate = name)
    #print "Generated.."
    #import time
    #time.sleep(15)
    return files

def peg_emitter(target, source, env):
    import sys
    #sys.path.append("src/mugen/parser")
    #sys.path.append('../../../src/mugen/parser')
    #sys.path.append(".")
    import peg, re, os
    # print "Node is %s" % node
    name = re.sub('\..*', '', str(os.path.basename(str(source[0].path))))
    directory = os.path.dirname(str(target[0].path))
    parser = peg.make_peg_parser(name)
    # Hack! just make the parser read a string directly
    where = '/tmp/scanner.tmp'
    stuff = open(where, 'w')
    stuff.write(source[0].get_contents())
    stuff.close()
    final = parser(where)
    # files = final.list_files(name, directory = directory)
    files = final.list_files(name)
    # print "Generating peg files for %s from %s to %s" % (target[0], source[0], directory)
    # print "Implicit dependancies %s" % ' '.join(files)
    # final.generate_cpp(separate = name, directory = directory)
    #print "Generated.."
    #import time
    #time.sleep(15)
    # print "Emitted %s" % files
    def isHeader(file):
        return '.h' in file
    return (target + [file for file in files if not isHeader(file)], source)
    #return (target + files, source)

# This is the architecture for the new SConstruct design
def newGetEnvironments():
    def osxEnvironments():
        # Build the environments here
        environment = False
        staticEnvironment = False
        return (environment, staticEnvironment)

    def unixEnvironments():
        environment = False
        staticEnvironment = False
        return (environment, staticEnvironment)

    if isOSX():
        return osxEnvironments()
    elif isLinux():
        return unixEnvironments()
    # ... do rest of platforms ...

import sys
sys.path.append('src/mugen/parser')
import peg

env = getEnvironment(getDebug())

#sys.path.append('scons')
#import unix
#env = unix.getEnvironment()
#unix.configure(env)

if useSDL():
    env['PAINTOWN_BACKEND'] = 'sdl'
elif useAllegro():
    env['PAINTOWN_BACKEND'] = 'allegro'
elif useAllegro5():
    env['PAINTOWN_BACKEND'] = 'allegro5'
else:
    env['PAINTOWN_BACKEND'] = 'unknown'

# Generates a single .cpp file from a .peg description
peg_builder = Builder(action = Action(peg_to_cpp, env['PEG_MAKE']),
                      suffix = '.cpp',
                      src_suffix = '.peg')

# Generates a .cpp file for each rule in the .peg description
peg_multiple_builder = Builder(action = Action(peg_to_cpp_multi, env['PEG_MAKE']),
                               suffix = '.cpp',
                               src_suffix = '.peg',
                               # source_scanner = Scanner(peg_scanner),
                               emitter = peg_emitter
                               )

env.Append(BUILDERS = {'Peg' : peg_builder})
env.Append(BUILDERS = {'PegMulti' : peg_multiple_builder})

if showTiming():
    cxxcom = env['CXXCOM']
    cccom = env['CCCOM']
    env.Replace(CXXCOM = 'misc/show-current-time %s' % cxxcom)
    env.Replace(CCCOM = 'misc/show-current-time %s' % cccom)

env['PAINTOWN_USE_PRX'] = useMinpspw() and usePrx()
if not useMinpspw() and not useNDS() and not useDingoo() and not useNacl() and not useAndroid() and not useIos():
    env['PAINTOWN_NETWORKING'] = True
    env.Append(CPPDEFINES = ['HAVE_NETWORKING'])
else:
    env['PAINTOWN_NETWORKING'] = False

def getDataPath():
    try:
        return ARGUMENTS['data']
    except KeyError:
        if useWii():
            # consistent with homebrew
            return '/apps/paintown/data'
        if useAndroid():
            return '/sdcard/paintown/data'
        if useMinpspw():
            return 'ms0:/psp/game150/paintown/data'
        else:
            return 'data'

debug = getDebug()
dataPath = getDataPath()

# This stuff should be moved to getEnvironment()
cflags = ['-Wall', '-Wno-unused-variable', '-Wno-unused-function']
cppflags = ['-Woverloaded-virtual']
cdefines = ['DATA_PATH=\\\"%s\\\"' % dataPath]
# cppflags = [ '-Woverloaded-virtual' ]

# env.Append( CCFLAGS = '-m32' )
# env.Append( LINKFLAGS = '-m32' )

env.Append(CCFLAGS = cflags,
           CXXFLAGS = cppflags,
           CPPPATH = ["#src"],
           CPPDEFINES = cdefines)

if env['PAINTOWN_NETWORKING']:
    env.Append(CPPPATH = ['#src/util/network/hawknl'])

# deprecated
def configEnvironment(env):
    if isWindows():
        def nothing(env):
            pass
        # dumb on windows has to have USE_ALLEGRO defined
        def doAllegro(env2):
            env2.Append(CPPDEFINES = ['USE_ALLEGRO'])
        def doSDL(env2):
            env2.Append(CPPDEFINES = ['USE_SDL'])
        env['paintown_enableAllegro'] = doAllegro
        env['paintown_enableSDL'] = doSDL
        return env
    else:
        custom_tests = {"CheckAllegro" : checkAllegro,
                        "CheckAllegro5" : checkAllegro5,
                        "CheckSDL" : checkSDL,
                        "CheckSDLMain" : checkSDLMain}
        config = env.Configure(custom_tests = custom_tests)
        if useAllegro():
            if not config.CheckAllegro():
                Exit(1)
        if useAllegro5():
            if not config.CheckAllegro5():
                Exit(1)
        if useSDL():
            if not config.CheckSDL():
                Exit(1)
            config.CheckSDLMain()
        return config.Finish()

# allegroEnvironment = configEnvironment(getEnvironment(debug))

def buildType(env):
    properties = []
    # Problem with command line too long under windows
    if isWindows() and useMinpspw():
        properties.append('psp')
    else:
        if useSDL():
            properties.append('sdl')
        if useMinpspw():
            properties.append('psp')
    if useAndroid():
        properties.append('android')
    if useIos():
        properties.append('ios')
    if usePs3():
        properties.append('ps3')
    if useNacl():
        properties.append('nacl%s' % env['PAINTOWN_NACL_ARCH'])
    if usePandora():
        properties.append('Pandora')
    if useNDS():
        properties.append('NDS')
    if useWii():
        properties.append('wii')
    if useAllegro():
        properties.append('allegro')
    if useDingoo():
        properties.append('dingoo')
    if useAllegro5():
        properties.append('allegro5')
    if getDebug():
        properties.append('debug')
    return '-'.join(properties)

import os.path
buildDir = os.path.join('build', buildType(env))
buildDirStatic = os.path.join('build/static', buildType(env))

#if getDebug():
#    env.Append(LIBS = ['gcov'])

# change this to if True if you want to profile paintown
if enableProfiled():
    env.Append(CCFLAGS = '-pg')
    env.Append(LINKFLAGS = '-pg')

custom_tests = {"CheckPython" : checkPython,
                "CheckRuby" : checkRuby,
                "CheckRTTI" : checkRTTI,
                "CheckAllegro" : checkAllegro,
                "CheckAllegro5" : checkAllegro5,
                "CheckPthreads" : checkPthreads,
                "CheckSDL" : checkSDL,
                "CheckSDLMain" : checkSDLMain,
                "CheckOgg" : checkNativeOgg,
                "CheckMpg123" : checkMpg123,
                "CheckMad" : checkMad}

def display_build_properties(env):
    color = 'light-green'
    properties = []
    if useAllegro():
        properties.append(colorize("Allegro", color))
    if useAllegro5():
        properties.append(colorize('Allegro5', color))
    if useSDL():
        properties.append(colorize("SDL", color))
    if getDebug():
        properties.append(colorize("Debug", color))
    if enableProfiled():
        properties.append(colorize("Profiling", color))
    if useWii():
        properties.append(colorize("Wii", color))
    if usePandora():
        properties.append('Pandora')
    if useNDS():
        properties.append(colorize("NDS", color))
    if useMinpspw():
        properties.append(colorize("PSP", color))
    if useAndroid():
        properties.append(colorize("ANDROID", color))
    if useIos():
        properties.append(colorize("IOS", color))
    if usePs3():
        properties.append(colorize("PS3", color))
    if useNacl():
        properties.append(colorize("NACL%s" % env['PAINTOWN_NACL_ARCH'], color))
    if useLLVM():
        properties.append(colorize("LLVM", color))
    if useIntel():
        properties.append(colorize("Intel", color))
    type = ' '.join(properties)
    if not isQuiet():
        print "Build type: %s" % type

display_build_properties(env)

env['PAINTOWN_TESTS'] = custom_tests
env['PAINTOWN_COLORIZE'] = colorize

if isWindows():
    staticEnv = env.Clone()

    windows_tests = {"CheckWindowsFreeType" : checkWindowsFreeType}
    config = env.Configure(custom_tests = windows_tests)
    config.CheckWindowsFreeType()
    # config.CheckPython()
    #if config.HasRuby():
    #    config.CheckRuby()
    
    env = config.Finish()

    if useAllegro():
        env.Append(CPPDEFINES = ['USE_ALLEGRO'])
        staticEnv.Append(CPPDEFINES = ['USE_ALLEGRO'])
        env.Append( LIBS = ['alleg', 'pthread', 'png', 'freetype', 'z', 'wsock32', 'regex.dll'] )
    elif useAllegro5():
        env.Append(CPPDEFINES = ['USE_ALLEGRO5'])
        staticEnv.Append(CPPDEFINES = ['USE_ALLEGRO5'])
        env.Append(LIBS = ['allegro-5.0.3-monolith-md', 'wsock32', 'regex.dll', 'z', 'freetype', 'png', 'psapi'])
    elif useSDL():
        if not useMinpspw() and not usePs3() and not useWii():
            env.Append(CPPDEFINES = ['USE_SDL'])
            # TODO: move this to a configure check
            env.Append(CPPPATH = ['c:/gcc4.5/include/SDL'])
            staticEnv.Append(CPPDEFINES = ['USE_SDL'])
            env.Append(LIBS = Split("""SDL pthread png user32 gdi32 winmm freetype z wsock32 regex.dll psapi mpg123 vorbisfile vorbis ogg"""))
            env['HAVE_MP3_MPG123'] = True
            env.Append(CPPDEFINES = ['HAVE_MP3_MPG123'])
            env.Append(CPPDEFINES = ['HAVE_OGG'])
        elif useMinpspw() or usePs3() or useWii():
            env.Append(CPPDEFINES = ['USE_SDL'])
            staticEnv.Append(CPPDEFINES = ['USE_SDL'])
    
    if not useMinpspw() and not usePs3() and not useWii():
        env.Append( CPPDEFINES = 'WINDOWS' )
        env.Append(LINKFLAGS = ['-static-libstdc++', '-static-libgcc'])
        if getDebug():
            env.Append( CCFLAGS = ['-mthreads'] )
            env.Append( LINKFLAGS = ['-mthreads'] )
        else:
            env.Append( CCFLAGS = ['-mwindows','-mthreads'] )
            env.Append( LINKFLAGS = ['-mwindows','-mthreads'] )
    
    if useSDL() and not useMinpspw() or not usePs3() or not useWii():
        staticEnv.Append(LIBS = ['SDL', 'pthread', 'png', 'freetype', 'z', 'wsock32', 'regex.dll'] )
    elif useAllegro():
        staticEnv.Append(LIBS = [ 'alleg', 'pthread', 'png', 'freetype', 'z', 'wsock32', 'regex.dll'] )
    
    staticEnv.Append(CPPDEFINES = 'WINDOWS')
else:
    staticEnv = env.Clone()

    import sys
    # find the system tool path by attaching SCons/Tool to everything
    def fix(q):
        return q + "/SCons/Tool"
    if useGch():
        env.Tool('gch', toolpath = ['misc'] + [fix(e) for e in sys.path if os.path.isdir(e)])
        if not getDebug():
            env['GCHFROMHCOMSTR'] = "%s %s" % (colorize('Compiling header', 'green'), colorize('$SOURCE', 'cyan'))

    if isOSX104():
        # Build a universal binary
        staticEnv['CXX'] = 'misc/g++'
        staticEnv['CC'] = 'misc/gcc'
    elif isLinux() and not useWii() and not useMinpspw() and not usePs3() and not useNDS() and not useDingoo() and not useAndroid() and not useNacl():
        staticEnv.Append(CPPDEFINES = 'LINUX')
        env.Append(CPPDEFINES = 'LINUX')
    
    config = env.Configure(custom_tests = custom_tests)
    try:
        # config.env.ParseConfig( 'allegro-config --libs --cflags' )
        if useAllegro() and not config.CheckAllegro():
            print "You need the development files for Allegro. Visit Allegro's website at http://alleg.sf.net or use your package manager to install them."
            Exit(1)

        if useAllegro5() and not config.CheckAllegro5():
            print "Allegro5 not found"
            Exit(1)

        config.CheckPthreads()

        #if not useWii() and not useMinpspw():
        #    env.Append(LIBS = [ 'pthread' ])
        #    staticEnv.Append(LIBS = [ 'pthread' ])

        if useSDL() and not useMinpspw() and not usePs3() and not useNDS() and not useAndroid() and not useNacl():
            if not config.CheckSDL():
                print "Install libsdl 1.2"
                Exit(1)
            config.CheckSDLMain()
        elif useMinpspw() or usePs3() or useNDS() or useAndroid() or useNacl():
            env.Append(CPPDEFINES = ['USE_SDL'])
            staticEnv.Append(CPPDEFINES = ['USE_SDL'])
            config.CheckSDLMain()
            #env.Append(CPPDEFINES = ['USE_SDL', 'USE_SDL_MAIN'])
            #staticEnv.Append(CPPDEFINES = ['USE_SDL', 'USE_SDL_MAIN'])
        
        if not usePs3() and not useNacl() and not useAndroid():
            safeParseConfig(config.env, 'freetype-config --libs --cflags')
            safeParseConfig(config.env, 'libpng-config --libs --ldflags --cflags')
        
            # staticEnv.ParseConfig( 'allegro-config --static --libs --cflags' )
            safeParseConfig(staticEnv, 'freetype-config --cflags')
            safeParseConfig(staticEnv, 'libpng-config --cflags')
    except OSError:
        pass

    
    ## This is a hack. Copy the static libraries to misc and then link
    ## those in, otherwise gcc will try to pick the .so's from /usr/lib
    if True or not isOSX():
        png = staticEnv.Install('misc', readExec('libpng-config --libdir' ) + '/libpng.a')
        staticEnv.Append(LIBS = [png])
    else:
        safeParseConfig(staticEnv, 'freetype-config --libs')
        safeParseConfig(staticEnv, 'libpng-config --libs --ldflags --cflags')

    #if useSDL():
    #    sdl = staticEnv.Install('misc', readExec('sdl-config --prefix') + '/lib/libSDL.a')
    #    sdlmain = staticEnv.Install('misc', readExec('sdl-config --prefix') + '/lib/libSDLmain.a')
    #    staticEnv.Append(LIBS = [sdl, sdlmain])
    #    staticEnv.ParseConfig('sdl-config --cflags')
    #    staticEnv.Append(CPPDEFINES = ['USE_SDL'])

    staticEnv.Append(LIBS = ['z','m'])
    if True or not isOSX():
        path = readExec('freetype-config --prefix') + '/lib/libfreetype.a'
        if file_exists(path):
            freetype = staticEnv.Install('misc', path)
            staticEnv.Append(LIBS = freetype)
        else:
            safeParseConfig(staticEnv, 'freetype-config --libs')

    if not config.TryCompile("int main(){ return 0; }\n", ".c"):
        print "You need a C compiler such as gcc installed"
        Exit( 1 )
    if not config.TryCompile("class a{public: a(){} }; int main(){ a * x = new a(); delete x; return 0; }\n", ".cpp" ):
        print "You need a C++ compiler such as g++ installed"
        Exit( 1 )
    #if not config.CheckHeader( 'allegro.h' ):
    #    print "You need the header files for Allegro. Get it from http://alleg.sf.net"
    #    Exit( 1 )
    if not config.CheckHeader('ft2build.h'):
        print "You need freetype. Install freetype and/or X11"
        Exit( 1 )
    if not config.CheckHeader('png.h'):
        print "You need libpng. Get it from http://www.libpng.org/pub/png/libpng.html"
        Exit(1)
    config.CheckRTTI()
    # config.CheckPython()
    config.CheckOgg()

    #if (useSDL() and not config.CheckMpg123()) or not useSDL():
    #    config.CheckMad()
    #    config.CheckMpg123()
    # Prefer mpg123 over mad because mpg123 can stream from the disk and
    # libmad is not so great at this
    if not config.CheckMpg123():
        config.CheckMad()

    #if config.HasRuby():
    #    config.CheckRuby()
    
    env = config.Finish()

    static_custom_tests = {"CheckPython" : checkPython,
                           "CheckRuby" : checkStaticRuby,
                           "CheckAllegro" : checkAllegro,
                           "CheckSDL" : checkStaticSDL,
                           "CheckSDLMain" : checkSDLMain,
                           "CheckPthreads" : checkPthreads,
                           "CheckRTTI" : checkRTTI}
    staticEnv['PAINTOWN_TESTS'] = static_custom_tests
    static_config = staticEnv.Configure(custom_tests = static_custom_tests)
    if useAllegro():
        static_config.CheckAllegro()
    if useSDL() and not useMinpspw() or usePs3():
        static_config.CheckSDL()
        static_config.CheckSDLMain()

    static_config.CheckPthreads()

    # static_config.CheckPython()
    #if static_config.HasRuby():
    #    static_config.CheckRuby()
    static_config.CheckRTTI()
    staticEnv = static_config.Finish()


# if not isWindows():
#    env.Append(CCFLAGS = ['-Werror'])
# staticEnv.Append(CCFLAGS = ['-Werror'])

env['PAINTOWN_BUILD_TESTS'] = True
use = env
shared = SConscript('src/SConscript', variant_dir = buildDir, exports = ['use'] );

staticEnv['PAINTOWN_BUILD_TESTS'] = False
use = staticEnv
static = SConscript('src/SConscript', variant_dir = buildDirStatic, exports = ['use'] )

scripts = Split("""
src/paintown-engine/script/modules/paintown.py
""");
# Don't care about ruby for now
# src/paintown-engine/script/modules/paintown.rb

Default(Install('data/scripts', scripts))

def psp_eboot(target, source, env):
    print "Make eboot from %s to %s" % (source[0].name, target[0].name)
    file = source[0].name
    env.Execute("mksfo 'Paintown' PARAM.SFO")
    env.Execute('psp-fixup-imports %s' % file)
    if usePrx():
        prx = '%s.prx' % file
        env.Execute('psp-prxgen %s %s' % (file, prx))
        env.Execute("pack-pbp EBOOT.PBP PARAM.SFO NULL NULL NULL NULL NULL %s NULL" % prx)
    else:
        env.Execute('psp-strip %s' % file)
        env.Execute("pack-pbp EBOOT.PBP PARAM.SFO data/psp/icon0.png NULL NULL data/psp/pic1.png NULL %s NULL" % file)
    return 0

def ps3_pkg(target, source, env):
    file = source[0].name
    app = 'UP0001-Paintown_00-0000000000000000'
    print "Creating PKG for %s" % file
    if getDebug():
        if env.Execute(Copy('%s.elf' % file, file)):
            print("Could not copy %s to %s.elf" % (file, file))
            return
    else:
        env.Execute('ppu-strip %s -o %s.elf' % (file, file))
    env.Execute('%s/bin/sprxlinker %s.elf' % (ps3devPath(), file))
    env.Execute('python %s/bin/fself.py %s.elf %s.fself' % (ps3devPath(), file, file))
    env.Execute('%s/bin/make_self %s.elf %s.self' % (ps3devPath(), file, file))
    env.Execute('mkdir -p pkg/USRDIR')
    env.Execute('%s/bin/make_self_npdrm %s.elf pkg/USRDIR/EBOOT.BIN %s' % (ps3devPath(), file, app))
    # env.Execute('cp %s %s.elf' % (file, file))
    env.Execute('cp data/ps3/icon0.png pkg/ICON0.PNG')
    env.Execute('cp data/ps3/SND0.AT3 pkg/')
    env.Execute('cp data/ps3/pic1.png pkg/PIC1.PNG')
    # env.Execute('python /opt/ps3dev/bin/fself.py -n %s.elf pkg/USRDIR/EBOOT.BIN' % file)
    env.Execute('python %s/bin/sfo.py --title "Paintown" --appid "PAINTOWN" -f %s/bin/sfo.xml pkg/PARAM.SFO' % (ps3devPath(), ps3devPath()))
    env.Execute('python %s/bin/pkg.py --contentid %s pkg/ %s.pkg' % (ps3devPath(), app, file))
    env.Execute('python %s/bin/pkg.py --contentid %s pkg/ %s.retail.pkg' % (ps3devPath(), app, file))
    env.Execute('%s/bin/package_finalize %s.retail.pkg' % (ps3devPath(), file));
    print "Sign pkg with tools from geohot or something (http://www.geohot.com)..."
    return 0

def wii_elf2dol(target, source, env):
    file = source[0].name
    print "Running elf2dol to create %s.dol for you" % file
    env.Execute('elf2dol %s boot.dol' % (file))
    print "Rename %s to %s.elf if you want to run in dolphin with debugging" % (file, file)
    return 0

def wii_show_data(target, source, env):
    print "Wii data path is %s" % colorize(getDataPath(), 'light-green')
    return 0

for i in shared:
    safe = env.Install('.', i)
    if useMinpspw():
        env.AddPostAction(safe, psp_eboot)
    if usePs3():
        env.AddPostAction(safe, ps3_pkg)
    if useWii():
        env.AddPostAction(safe, wii_elf2dol)
        env.AddPostAction(safe, wii_show_data)
    Default(safe)

for i in static:
    installed = staticEnv.InstallAs(i[0].name + '-static', i)
    Alias('static', installed)

#if isQuiet():
#    Progress(['-\r', '\\\r', '|\r', '/\r'], interval=5)
