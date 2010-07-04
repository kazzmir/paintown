import os

def isPlatform(platform):
    import sys
    return platform in sys.platform

def isWindows():
    return isPlatform("win32")

def isOSX():
    return isPlatform("darwin")

def isLinux():
    return isPlatform("linux")

def noColors():
    try:
        return int(ARGUMENTS['colors']) == 0
    except KeyError:
        return False

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
useGch = makeUseArgument('gch', True)
usePrx = makeUseEnvironment('prx', False)
isVerbose = makeUseArgument('verbose', False)
useIntel = makeUseEnvironment('intel', False)
useMinpspw = makeUseEnvironment('minpspw', False)
useWii = makeUseEnvironment('wii', False)
useLLVM = makeUseEnvironment('llvm', False)
enableProfiled = makeUseEnvironment('PROFILE', False)

def checkLex(context):
    context.Message("Checking for flex... ")
    out = context.TryAction("flex -V")
    if out[0] != 1:
        print "Failed. Output was '%s'" % out[1]
    ret = out[0] == 1
    context.Result(ret)
    return ret

def checkYacc(context):
    context.Message("Checking for yacc... ")
    out = context.TryAction("yacc src/garbage.yy")
    if out[0] != 1:
        print "Failed. Output was '%s'" % out[1]
    ret = out[0] == 1
    context.Result(ret)
    return ret

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
    finally:
        context.sconf.env['SPAWN'] = spawn

    foo = 0

    if not result:
        context.sconf.env = tmp
        foo = 1

    context.Result(foo)
    return foo

def checkAllegro5(context):
    context.Message("Checking for Allegro 5 ... ")
    tmp = context.env.Clone()
    env = context.env
    try:
        env.ParseConfig('pkg-config allegro-4.9 --cflags --libs')
        env.Append(CPPDEFINES = ['USE_ALLEGRO5'])
        context.Result(1)
        return 1
    except:
        context.sconf.env = tmp
    context.Result(0)
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
            env.ParseConfig('sdl-config --cflags --libs')
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
            libs = env['LIBS']
            env.Replace(LIBS = [])
            env.ParseConfig('sdl-config --cflags --libs')
            env.Append(LIBS = libs)
            env.Append(CPPDEFINES = ['USE_SDL'])
            m = build('b')
            if m:
                return True
            else:
                raise Exception("Couldn't build it")
        except Exception, e:
            print "Moving libraries failed! because '%s'" % e
            context.sconf.env = tmp
            return False

    ok = int(tryNormal() or tryMoveLibs())
    context.Result(ok)
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
    
    context.Result(ok)
    return ok

def checkStaticSDL(context):
    context.Message("Checking for static SDL... ")
    env = context.env

    env.ParseConfig('sdl-config --static-libs --cflags')
    env.Append(CPPDEFINES = ['USE_SDL'])

    if False:
        sdl = env.Install('misc', readExec('sdl-config --prefix') + '/lib/libSDL.a')
        env.Append(LIBS = [sdl])
        env.ParseConfig('sdl-config --cflags')
        env.Append(CPPDEFINES = ['USE_SDL'])
        if isOSX():
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
    context.Result(1)
    return 1

def checkAllegro(context):
    context.Message("Checking for Allegro... ")
    tmp = context.env.Clone()
    env = context.env

    ok = 1
    try:
        def enableAllegro(env2):
            env2.ParseConfig('allegro-config --cflags --libs')
            env2.Append(CPPDEFINES = ['USE_ALLEGRO'])
        env.ParseConfig('allegro-config --cflags --libs')
        env['paintown_enableAllegro'] = enableAllegro
        env.Append(CPPDEFINES = ['USE_ALLEGRO'])
        ok = context.TryLink("""
        #include <allegro.h>
        int main(int argc, char ** argv){
          install_allegro(0, NULL, NULL);
          return 0;
        }
        END_OF_MAIN()
    """, ".c")

    except OSError:
        ok = 0 

    if not ok:
        context.sconf.env = tmp

    context.Result(ok)
    return ok

def checkPthreads(context):
    context.Message("Checking for threads... ")
    if useAllegro():
        env = context.env
        env.Append(LIBS = ['pthread'])
        context.Message(" pthreads")
        context.Result(1)
        return 1
    if useSDL():
        context.Message(" SDL threads")
        context.Result(1)
        return 1
    context.Message(" defaulting to pthreads")
    context.Result(1)
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
            env.ParseConfig('pkg-config vorbisfile --libs --cflags')
        except OSError:
            context.sconf.env = tmp
            context.Result(0)
            return 0

    ret = context.TryLink("""
        #include <vorbis/vorbisfile.h>
        #include <stdio.h>
        int main(int argc, char ** argv){
          OggVorbis_File ovf;
          FILE * f;
          ov_open_callbacks(f, &ovf, 0, 0, OV_CALLBACKS_DEFAULT);
          return 0;
        }
    """, ".c")

    if not ret:
        context.sconf.env = tmp

    context.Result(ret)
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
    if isOSX():
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

    context.Result(ret)
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
        context.Result(0)
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

    context.Result(ret)
    return ret

def checkStaticRuby(context):
    context.Message("Checking if ruby is statically embeddable... ")
    if not canRunRuby(context):
        context.Result(0)
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

    context.Result(ret)
    return ret

def canRunRuby(context):
    (ok, stuff) = context.TryAction(Action("ruby -v"))
    return ok == 1

def checkRunRuby(context):
    # just fail for now
    context.Result(0)
    return 0
    context.Message("Checking if we can run ruby... ")
    (ok, stuff) = context.TryAction(Action("ruby -v"))
    context.Result(ok)
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
                context.Result(0)
                return 0
        else:
            context.Message("don't know how to find freetype for a non-mingw compiler")
            context.Result(0)
            return 0

    context.Result(1)
    return 1

def useSDL():
    def byEnv():
        try:
            return os.environ['SDL'] == '1'
        except KeyError:
            return False

    def byArgument():
        try:
            return int(ARGUMENTS['sdl']) == 1
        except KeyError:
            return False

    return byEnv() or byArgument()

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
    return not useSDL() and not useAllegro5()

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
              'dark-grey': "1:30",
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
    def llvm(env):
        #env['CC'] = 'llvm-gcc'
        #env['CXX'] = 'llvm-g++'
        # Use these eventually
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
        cflags = []
        if debug:
            # for gcov:
            # ['-fprofile-arcs', '-ftest-coverage']
            # cflags.append( ['-g3','-ggdb', '-Werror'])
            cflags.append( ['-g3','-ggdb'])
        else:
            cflags.append(['-O2'])

        if isCygwin():
            import SCons.Tool.zip
            env = Environment(ENV = os.environ, tools = ['mingw'])
            env['CXX'] = 'C:\\MinGW\\bin\\g++.exe'
            env['CC'] = 'C:\\MinGW\\bin\\gcc.exe'
            env['AR'] = 'C:\\MinGW\\bin\\ar.exe'
            SCons.Tool.zip.generate(env)
            return env
        elif useMingw():
            return Environment(ENV = os.environ, CCFLAGS = cflags, tools = ['mingw', 'zip'])
        else:
            if useIntel():
                print "Using the intel compiler"
                return intel(Environment(ENV = os.environ, CCFLAGS = cflags))
            elif useWii():
                if isWindows():
                    return wii(Environment(ENV = os.environ, CCFLAGS = cflags, tools = ['mingw']))
                return wii(Environment(ENV = os.environ, CCFLAGS = cflags))
            elif useMinpspw():
                return minpspw(Environment(ENV = os.environ, CCFLAGS = cflags, tools = ['mingw']))
            elif useLLVM():
                return llvm(Environment(ENV = os.environ, CCFLAGS = cflags))
            else:
                return Environment(ENV = os.environ, CCFLAGS = cflags)
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
    
env = getEnvironment(getDebug())
env['PAINTOWN_USE_PRX'] = usePrx()
if not useWii() and not useMinpspw():
    env['PAINTOWN_NETWORKING'] = True
    env.Append(CPPDEFINES = ['HAVE_NETWORKING'])
else:
    env['PAINTOWN_NETWORKING'] = False

def getDataPath():
    try:
        return ARGUMENTS['data']
    except KeyError:
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
    env.Append(CPPPATH = ['#src/hawknl'])

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

def buildType(dir):
    properties = [dir]
    # Problem with command line too long under windows
    if isWindows() and useMinpspw():
        properties.append('psp')
    else:
        if useSDL():
            properties.append('sdl')
        if useMinpspw():
            properties.append('psp')
    if useWii():
        properties.append('wii')
    if useAllegro5():
        properties.append('allegro5')
    if getDebug():
        properties.append('debug')
    return '-'.join(properties)

buildDir = buildType('build')
buildDirStatic = buildType('build-static')

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
                "CheckPthreads" : checkPthreads,
                "CheckSDL" : checkSDL,
                "CheckSDLMain" : checkSDLMain,
                "CheckOgg" : checkNativeOgg}

env['PAINTOWN_TESTS'] = custom_tests

if isWindows():
    staticEnv = env.Clone()

    windows_tests = {"CheckWindowsFreeType" : checkWindowsFreeType}
    config = env.Configure(custom_tests = windows_tests)
    config.CheckWindowsFreeType()
    # config.CheckPython()
    #if config.HasRuby():
    #    config.CheckRuby()
    # if not config.CheckLex():
    #     print "Can't find lex"
    #     Exit(1)
    # the yacc check gets into an infinite loop when checking under
    # cygwin, so just disable it for now
    # if False and not config.CheckYacc():
    #     print "Can't find yacc"
    #     Exit(1)
    env = config.Finish()

    if useAllegro():
        env.Append(CPPDEFINES = ['USE_ALLEGRO'])
        staticEnv.Append(CPPDEFINES = ['USE_ALLEGRO'])
        env.Append( LIBS = ['alleg', 'pthread', 'png', 'freetype', 'z', 'wsock32', 'regex.dll'] )
    
    elif useSDL():
        if not useMinpspw() and not useWii():
            env.Append(CPPDEFINES = ['USE_SDL'])
            # TODO: move this to a configure check
            env.Append(CPPPATH = ['c:/gcc4.5/include/SDL'])
            staticEnv.Append(CPPDEFINES = ['USE_SDL'])
            env.Append( LIBS = ['SDL', 'pthread', 'png', 'user32', 'gdi32', 'winmm', 'freetype', 'z', 'wsock32', 'regex.dll'] )
        elif useMinpspw() or useWii():
            env.Append(CPPDEFINES = ['USE_SDL'])
            staticEnv.Append(CPPDEFINES = ['USE_SDL'])
    
    if not useMinpspw() and not useWii():
        env.Append( CPPDEFINES = 'WINDOWS' )
        env.Append(LINKFLAGS = ['-static-libstdc++'])
        if getDebug():
            env.Append( CCFLAGS = ['-mthreads'] )
            env.Append( LINKFLAGS = ['-mthreads'] )
        else:
            env.Append( CCFLAGS = ['-mwindows','-mthreads'] )
            env.Append( LINKFLAGS = ['-mwindows','-mthreads'] )
    
    if useSDL() and not useMinpspw() or not useWii():
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

    if isOSX():
        # Build a universal binary
        staticEnv['CXX'] = 'misc/g++'
        staticEnv['CC'] = 'misc/gcc'
    elif isLinux() and not useWii() and not useMinpspw():
        staticEnv.Append(CPPDEFINES = 'LINUX')
        env.Append(CPPDEFINES = 'LINUX')
    
    config = env.Configure(custom_tests = custom_tests)
    try:
        # config.env.ParseConfig( 'allegro-config --libs --cflags' )
        if useAllegro() and not config.CheckAllegro():
            print "You need the development files for Allegro. Visit Allegro's website at http://alleg.sf.net or use your package manager to install them."

        config.CheckPthreads()

        #if not useWii() and not useMinpspw():
        #    env.Append(LIBS = [ 'pthread' ])
        #    staticEnv.Append(LIBS = [ 'pthread' ])

        if useSDL() and not useMinpspw():
            config.CheckSDL()
            config.CheckSDLMain()
        elif useMinpspw():
            env.Append(CPPDEFINES = ['USE_SDL'])
            staticEnv.Append(CPPDEFINES = ['USE_SDL'])
            config.CheckSDLMain()
            #env.Append(CPPDEFINES = ['USE_SDL', 'USE_SDL_MAIN'])
            #staticEnv.Append(CPPDEFINES = ['USE_SDL', 'USE_SDL_MAIN'])

        config.env.ParseConfig( 'freetype-config --libs --cflags' )
        config.env.ParseConfig( 'libpng-config --libs --cflags' )
        
        # staticEnv.ParseConfig( 'allegro-config --static --libs --cflags' )
        staticEnv.ParseConfig( 'freetype-config --cflags' )
        staticEnv.ParseConfig( 'libpng-config --cflags' )
    except OSError:
        pass

    
    ## This is a hack. Copy the static libraries to misc and then link
    ## those in, otherwise gcc will try to pick the .so's from /usr/lib
    png = staticEnv.Install( 'misc', readExec( 'libpng-config --libdir' ) + '/libpng.a' )
    staticEnv.Append(LIBS = [png])

    #if useSDL():
    #    sdl = staticEnv.Install('misc', readExec('sdl-config --prefix') + '/lib/libSDL.a')
    #    sdlmain = staticEnv.Install('misc', readExec('sdl-config --prefix') + '/lib/libSDLmain.a')
    #    staticEnv.Append(LIBS = [sdl, sdlmain])
    #    staticEnv.ParseConfig('sdl-config --cflags')
    #    staticEnv.Append(CPPDEFINES = ['USE_SDL'])

    staticEnv.Append( LIBS = ['z','m'] )
    freetype = staticEnv.Install( 'misc', readExec('freetype-config --prefix') + '/lib/libfreetype.a' )
    staticEnv.Append(LIBS = freetype)

    if not config.TryCompile("int main(){ return 0; }\n", ".c"):
        print "You need a C compiler such as gcc installed"
        Exit( 1 )
    if not config.TryCompile("class a{public: a(){} }; int main(){ a * x = new a(); delete x; return 0; }\n", ".cpp" ):
        print "You need a C++ compiler such as g++ installed"
        Exit( 1 )
    #if not config.CheckHeader( 'allegro.h' ):
    #    print "You need the header files for Allegro. Get it from http://alleg.sf.net"
    #    Exit( 1 )
    if not config.CheckHeader( 'ft2build.h' ):
        print "You need freetype. Install freetype and/or X11"
        Exit( 1 )
    if not config.CheckHeader( 'png.h' ):
        print "You need libpng. Get it from http://www.libpng.org/pub/png/libpng.html"
        Exit(1)
    config.CheckRTTI()
    # config.CheckPython()
    config.CheckOgg()
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
    if useSDL() and not useMinpspw():
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

use = env
shared = SConscript( 'src/SConstruct', build_dir = buildDir, exports = ['use'] );

use = staticEnv
static = SConscript( 'src/SConstruct', build_dir = buildDirStatic, exports = ['use'] )

scripts = Split("""
src/script/modules/paintown.py
src/script/modules/paintown.rb
""");

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
        env.Execute("pack-pbp EBOOT.PBP PARAM.SFO data/menu/psp/icon0.png NULL NULL data/menu/psp/pic1.png NULL %s NULL" % file)
    return 0

for i in shared:
    safe = env.Install('.', i)
    if useMinpspw():
        env.AddPostAction(safe, psp_eboot)
    Default(safe)

for i in static:
    Alias('static', env.InstallAs( i[0].name + '-static', i ))
