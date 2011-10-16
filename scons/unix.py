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

def configure(environment):
    custom_tests = {"CheckCompiler": checkCompiler}
    config = environment.Configure(custom_tests = custom_tests)

    if not config.CheckCompiler():
        print("No c++ compiler found. Install gcc or clang")
        Exit(1)

    return config.Finish()

def getEnvironment():
    import utils
    environment = Environment(ENV = os.environ)
    peg_color = 'light-cyan'
    environment['PAINTOWN_PLATFORM'] = 'unix'
    environment['PAINTOWN_BACKEND'] = 'sdl'
    environment['PAINTOWN_USE_PRX'] = False
    environment['PAINTOWN_TESTS'] = {'CheckPython': checkPython}
    environment['PAINTOWN_COLORIZE'] = utils.colorize
    environment['PAINTOWN_NETWORKING'] = True
    environment['LIBS'] = []
    environment['PEG_MAKE'] = "%s %s" % (utils.colorize('Creating peg parser', peg_color), utils.colorize('$TARGET', 'light-blue'))
    environment.Append(BUILDERS = {'Peg' : utils.pegBuilder(environment)})
    environment.Append(CPPPATH = ['#src', '#src/util/network/hawknl'])
    environment.Append(CPPDEFINES = ['USE_SDL'])
    environment.ParseConfig('sdl-config --libs --cflags')
    environment.ParseConfig('freetype-config --libs --cflags')
    environment.ParseConfig('libpng-config --libs --cflags')
    return configure(environment)
