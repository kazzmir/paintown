import os

def isWindows():
    import re
    import sys
    return "win32" in sys.platform

def isOSX():
    import re
    import sys
    return "darwin" in sys.platform

def isLinux():
    import re
    import sys
    return "linux" in sys.platform

def noColors():
    try:
        return int(ARGUMENTS['colors']) == 0
    except KeyError:
        return False

def useGch():
    try:
        return int(ARGUMENTS['gch']) == 1
    except KeyError:
        return True

def useIntel():
    try:
        return int(os.environ['intel'])
    except KeyError:
        return False

def useLLVM():
    try:
        return int(os.environ['llvm'])
    except KeyError:
        return False

# todo: read from an environment variable or something
def enableProfiled():
    try:
        return int(os.environ[ 'PROFILE' ])
    except KeyError:
        return 0

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

def checkSDL(context):
    context.Message("Checking for SDL... ")
    tmp = context.env.Clone()
    env = context.env

    ok = 0
    try:
        def enableSDL(env2):
            env2.ParseConfig('sdl-config --cflags --libs')
        env.ParseConfig('sdl-config --cflags --libs')
        env['enableSDL'] = enableSDL
        env.Append(CPPDEFINES = ['USE_SDL'])
        ok = context.TryLink("""
        #include <SDL.h>
        int main(int argc, char ** argv){
          return SDL_Init(0);
        }
    """, ".c")

    except OSError:
        ok = 1 

    if not ok:
        context.sconf.env = tmp

    context.Result(ok)
    return ok

def checkAllegro(context):
    context.Message("Checking for Allegro... ")
    tmp = context.env.Clone()
    env = context.env

    ok = 0
    try:
        def enableAllegro(env2):
            env2.ParseConfig('allegro-config --cflags --libs')
        env.ParseConfig('allegro-config --cflags --libs')
        env['enableAllegro'] = enableAllegro
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
        ok = 1 

    if not ok:
        context.sconf.env = tmp

    context.Result(ok)
    return ok

def checkOgg(context):
    context.Message("Checking for ogg and vorbis... ")
    tmp = context.env.Clone()
    env = context.env
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

def useSDL():
    try:
        return os.environ['SDL'] == '1'
    except KeyError:
        return False

def useAllegro():
    return not useSDL()

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
        return os.popen( program ).readline().replace("\n",'')
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
    env['CCCOMSTR'] = "%s %s" % (colorize('Compiling c file', 'light-green'), colorize('$SOURCE', 'light-blue'))
    env['SHCCCOMSTR'] = "%s %s" % (colorize('Compiling c file', 'light-green'), colorize('$SOURCE', 'light-blue'))
    env['CXXCOMSTR'] = "%s %s" % (colorize('Compiling c++ file', 'light-green'), colorize('$SOURCE', 'light-blue'))
    env['SHCXXCOMSTR'] = "%s %s" % (colorize('Compiling c++ file', 'light-green'), colorize('$SOURCE', 'light-blue'))
    env['LINKCOMSTR'] = "%s %s" % (colorize('Linking', link_color), colorize('$TARGET', 'light-blue'))
    env['SHLINKCOMSTR'] = "%s %s" % (colorize('Linking', link_color), colorize('$TARGET', 'light-blue'))
    env['ARCOMSTR'] = "%s %s" % (colorize('Building library', ar_color), colorize('$TARGET', 'light-blue'))
    env['RANLIBCOMSTR'] = "%s %s" % (colorize('Indexing library', ranlib_color), colorize('$TARGET', 'light-blue'))
    return env

def getEnvironment(debug):
    def intel(env):
        env['CC'] = 'icc'
        env['CXX'] = 'icpc'
        # '-Werror-all', '-Weffc++'
        env.Append(CCFLAGS = ['-wd981', '-wd2259'],
                   CXXFLAGS = ['-wd981', '-wd271',
                               '-wd383', '-wd869',
                               '-wd1599'])
        return env
    def llvm(env):
        env['CC'] = 'llvm-gcc'
        env['CXX'] = 'llvm-g++'
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
            cflags.append( ['-g3','-ggdb', '-Werror'])
        else:
            cflags.append( '-O2' )

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
            elif useLLVM():
                return llvm(Environment(ENV = os.environ, CCFLAGS = cflags))
            else:
                return Environment(ENV = os.environ, CCFLAGS = cflags)
    if not debug:
        return less_verbose(raw())
    else:
        return raw()
        
if isWindows():
    print "Try 'scons env=mingw' if you want to use mingw's gcc instead of visual studio or borland"
    if not isCygwin():
        print "Cygwin not detected. If are you using cygwin set"
        print "export CYGWIN=1"
    else:
        print "Cygwin detected"
    
env = getEnvironment(getDebug())

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

env['LEX'] = 'flex'

# env.Append( CCFLAGS = cflags, CXXFLAGS = cppflags, CPPPATH = [ ".", 'sockets' ] )
env.Append( CCFLAGS = cflags,
            CXXFLAGS = cppflags,
            CPPPATH = [ "#src", '#src/hawknl' ],
            CPPDEFINES = cdefines )

def buildDumb(where, env):
    return SConscript("src/dumb/SConscript", build_dir = '%s/dumb' % where, exports = 'env')

def buildHawknl(where, env):
    return SConscript("src/hawknl/SConscript", build_dir = '%s/hawknl' % where, exports = 'env')

def configEnvironment(env):
    if isWindows():
        def nothing(env):
            pass
        env['enableAllegro'] = nothing
        env['enableSDL'] = nothing
        return env
    else:
        custom_tests = {"CheckAllegro" : checkAllegro,
                        "CheckSDL" : checkSDL}
        config = env.Configure(custom_tests = custom_tests)
        if useAllegro():
            if not config.CheckAllegro():
                Exit(1)
        if useSDL():
            if not config.CheckSDL():
                Exit(1)
        return config.Finish()

allegroEnvironment = configEnvironment(getEnvironment(debug))

dumbEnv = getEnvironment(debug)
if useAllegro():
    allegroEnvironment['enableAllegro'](dumbEnv)
if useSDL():
    allegroEnvironment['enableSDL'](dumbEnv)
hawkEnv = getEnvironment(debug)
dumbStaticEnv = getEnvironment(debug)
if useAllegro():
    allegroEnvironment['enableAllegro'](dumbStaticEnv)
if useSDL():
    allegroEnvironment['enableSDL'](dumbStaticEnv)
hawkStaticEnv = getEnvironment(debug)

# if you dont care about building a universal binary then disable this
# block of code
if isOSX():
    dumbStaticEnv[ 'CXX' ] = 'misc/g++'
    dumbStaticEnv[ 'CC' ] = 'misc/gcc'
    hawkStaticEnv[ 'CXX' ] = 'misc/g++'
    hawkStaticEnv[ 'CC' ] = 'misc/gcc'
    hawkStaticEnv.Append( CPPDEFINES = 'MACOSX' )
    hawkEnv.Append( CPPDEFINES = 'MACOSX' )

hawknl = buildHawknl('build', hawkEnv)
dumb = buildDumb( 'build', dumbEnv )

hawknl_static = buildHawknl('build-static', hawkStaticEnv)
dumb_static = buildDumb('build-static', dumbStaticEnv)

#if getDebug():
#    env.Append(LIBS = ['gcov'])

# change this to if True if you want to profile paintown
if enableProfiled():
    env.Append(CCFLAGS = '-pg')
    env.Append(LINKFLAGS = '-pg')


# env.Append( LIBS = [ 'aldmb', 'dumb' ] );

custom_tests = {"CheckPython" : checkPython,
                "CheckRuby" : checkRuby,
                "CheckRTTI" : checkRTTI,
                "CheckAllegro" : checkAllegro,
                "CheckSDL" : checkSDL,
                "CheckOgg" : checkOgg}

env['PAINTOWN_TESTS'] = custom_tests

if isWindows():
    staticEnv = env.Clone()

    config = env.Configure(custom_tests = custom_tests)
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
    
    staticEnv.Append( LIBS = [hawknl_static, dumb_static] )
    env.Append( LIBS = [dumb,hawknl] )
    
    env.Append( LIBS = ['alleg', 'pthreadGC2', 'png', 'freetype', 'z', 'wsock32', 'regex.dll'] )
    env.Append( CPPDEFINES = 'WINDOWS' )
    if getDebug():
        env.Append( CCFLAGS = ['-mthreads'] )
        env.Append( LINKFLAGS = ['-mthreads'] )
    else:
        env.Append( CCFLAGS = ['-mwindows','-mthreads'] )
        env.Append( LINKFLAGS = ['-mwindows','-mthreads'] )

    staticEnv.Append(LIBS = [ 'alleg', 'pthreadGC2', 'png', 'freetype', 'z', 'wsock32', 'regex.dll'] )
    staticEnv.Append(CPPDEFINES = 'WINDOWS')
else:
    staticEnv = env.Clone()
    env.Append( LIBS = [ 'pthread' ] )
    staticEnv.Append( LIBS = [ 'pthread' ] )

    import sys
    # find the system tool path by attaching SCons/Tool to everything
    def fix(q):
        return q + "/SCons/Tool"
    if useGch():
        env.Tool('gch', toolpath = ['misc'] + [fix(e) for e in sys.path if os.path.isdir(e)])
        if not getDebug():
            env['GCHFROMHCOMSTR'] = "%s %s" % (colorize('Compiling header', 'green'), colorize('$SOURCE', 'cyan'))

    #try:
    #    dumbStaticEnv.ParseConfig( 'allegro-config --cflags' )
    #except OSError:
    #    pass

    if isOSX():
        staticEnv[ 'CXX' ] = 'misc/g++'
        staticEnv[ 'CC' ] = 'misc/gcc'
    elif isLinux():
        staticEnv.Append(CPPDEFINES = 'LINUX')
        env.Append(CPPDEFINES = 'LINUX')
    
    config = env.Configure(custom_tests = custom_tests)
    try:
        # config.env.ParseConfig( 'allegro-config --libs --cflags' )
        if useAllegro() and not config.CheckAllegro():
            print "You need the development files for Allegro. Visit Allegro's website at http://alleg.sf.net or use your package manager to install them."

        if useSDL():
            config.CheckSDL()

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
    freetype = staticEnv.Install( 'misc', readExec( 'freetype-config --prefix' ) + '/lib/libfreetype.a' )
    staticEnv.Append( LIBS = [png,'z','m'] )
    staticEnv.Append( LIBS = freetype )

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
        Exit( 1 )
    config.CheckRTTI()
    # config.CheckPython()
    config.CheckOgg()
    #if config.HasRuby():
    #    config.CheckRuby()
    
    env = config.Finish()

    static_custom_tests = {"CheckPython" : checkPython,
                           "CheckRuby" : checkStaticRuby,
                           "CheckAllegro" : checkAllegro,
                           "CheckRTTI" : checkRTTI}
    staticEnv['PAINTOWN_TESTS'] = static_custom_tests
    static_config = staticEnv.Configure(custom_tests = static_custom_tests)
    static_config.CheckAllegro()
    # static_config.CheckPython()
    #if static_config.HasRuby():
    #    static_config.CheckRuby()
    static_config.CheckRTTI()
    staticEnv = static_config.Finish()

    staticEnv.Prepend( LIBS = [hawknl_static, dumb_static] )
    env.Append( LIBS = [dumb,hawknl] )

# if not isWindows():
#    env.Append(CCFLAGS = ['-Werror'])
# staticEnv.Append(CCFLAGS = ['-Werror'])

use = env
shared = SConscript( 'src/SConstruct', build_dir='build', exports = ['use'] );

use = staticEnv
static = SConscript( 'src/SConstruct', build_dir='build-static', exports = ['use'] )

scripts = Split("""
src/script/modules/paintown.py
src/script/modules/paintown.rb
""");

Default(Install('data/scripts', scripts))

for i in shared:
    Default(env.Install( '.', i ))

for i in static:
    Alias('static',env.InstallAs( i[0].name + '-static', i ))
