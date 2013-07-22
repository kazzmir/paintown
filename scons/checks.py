import utils

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

    context.Result(utils.colorResult(foo))
    return foo

def checkAllegro5(debug):
    use_debug = [""]
    if debug:
        use_debug[0] = "-debug"
    def make(context):
        context.Message("Checking for Allegro 5 ... ")
        tmp = context.env.Clone()
        env = context.env
        def find(version):
            context.Message(str(version))
            try:
                def make(name):
                    return '%s%s-%s' % (name, use_debug[0], version)
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
            except Exception, e:
                print e
                return False
        try:
            ok = 0
            # if find(5.1) or find(5.0):
            if find(5):
                ok = 1
            context.Result(utils.colorResult(ok))
            return ok
        except:
            context.sconf.env = tmp
        context.Result(utils.colorResult(0))
        return 0

    return make

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
        main = env.StaticLibrary('src/util/graphics/sdl/SDLMain.m')
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

def checkSDLMain(context):
    context.Message("Checking for SDL main... ")
    tmp = context.env.Clone()
    env = context.env

    env['HAVE_SDL_MAIN'] = False

    ok = False
    if utils.useAndroid():
        ok = True
    else:
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
        env['HAVE_SDL_MAIN'] = True
    
    context.Result(utils.colorResult(ok))
    return ok

def checkStaticSDL(context):
    context.Message("Checking for static SDL... ")
    env = context.env

    try:
        utils.safeParseConfig(env, 'sdl-config --static-libs --cflags')
        env.Append(CPPDEFINES = ['USE_SDL'])
    except Exception:
        context.Result(utils.colorResult(0))
        return 0

    if False:
        sdl = env.Install('misc', readExec('sdl-config --prefix') + '/lib/libSDL.a')
        env.Append(LIBS = [sdl])
        utils.safeParseConfig(env, 'sdl-config --cflags')
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
    context.Result(utils.colorResult(1))
    return 1

def checkMpg123(context):
    context.Message("Checking for libmpg123... ")
    tmp = context.env.Clone()
    env = context.env
    env['HAVE_MP3_MPG123'] = True
    env.Append(CPPDEFINES = ['HAVE_MP3_MPG123'])
    (ok, stuff) = context.TryAction(env.Action("pkg-config --version"))
    if ok:
        try:
            utils.safeParseConfig(env,'pkg-config libmpg123 --libs --cflags') 
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
 
# Alternatively use libmad if mpg123 is not available
def checkMad(context):
    context.Message("Checking for libmad... ")
    tmp = context.env.Clone()
    env = context.env
    env['HAVE_MP3_MAD'] = True
    env.Append(CPPDEFINES = ['HAVE_MP3_MAD'])
    def tryPkgConfig():
        (ok, stuff) = context.TryAction(env.Action("pkg-config --version"))
        if ok:
            try:
                utils.safeParseConfig(env, 'pkg-config mad --libs --cflags') 
                return True
            except OSError:
                # context.sconf.env = tmp
                # context.Result(utils.colorResult(0))
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

    context.Result(utils.colorResult(ret))
    return ret

def checkPthreads(context):
    context.Message("Checking for threads... ")
    if utils.useAndroid() or utils.useAndroidX86():
        context.Message(" android threads")
        context.Result(utils.colorResult(1))
        return 1
    if utils.useAllegro():
        env = context.env
        env.Append(LIBS = ['pthread'])
        context.Message(" pthreads")
        context.Result(utils.colorResult(1))
        return 1
    if utils.useAllegro5():
        env = context.env
        env.Append(LIBS = ['pthread'])
        context.Message(' pthreads')
        context.Result(utils.colorResult(1))
        return 1
    if utils.useSDL():
        # context.Message(" SDL threads")
        env = context.env
        env.Append(LIBS = ['pthread'])
        context.Message(' pthreads')
        context.Result(utils.colorResult(1))
        return 1
    context.Message(" defaulting to pthreads")
    context.Result(utils.colorResult(1))
    return 1
        #if not useWii() and not useMinpspw():
        #    env.Append(LIBS = [ 'pthread' ])

def checkNativeOgg(context):
    context.Message("Checking for ogg and vorbis... ")
    tmp = context.env.Clone()
    env = context.env
    env['HAVE_OGG'] = True
    env.Append(CPPDEFINES = ['HAVE_OGG'])
    (ok, stuff) = context.TryAction(env.Action("pkg-config --version"))
    if ok:
        try:
            utils.safeParseConfig(env, 'pkg-config vorbisfile --libs --cflags')
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
    env.Replace(LIBS = [new_libs])
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
        context.Result(utils.colorResult(0))
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

    context.Result(utils.colorResult(ret))
    return ret

def checkStaticRuby(context):
    context.Message("Checking if ruby is statically embeddable... ")
    if not canRunRuby(context):
        context.Result(utils.colorResult(0))
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

    context.Result(utils.colorResult(ret))
    return ret

def canRunRuby(context):
    (ok, stuff) = context.TryAction(Action("ruby -v"))
    return ok == 1

def checkRunRuby(context):
    # just fail for now
    context.Result(utils.colorResult(0))
    return 0
    context.Message("Checking if we can run ruby... ")
    (ok, stuff) = context.TryAction(Action("ruby -v"))
    context.Result(utils.colorResult(ok))
    return ok

