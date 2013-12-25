from SCons.Script import ARGUMENTS
import os

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

def isPlatform(platform):
    import sys
    return platform in sys.platform

def isWindows():
    return isPlatform("win32")

def isLinux():
    return isPlatform("linux")

def isOSX104():
    import platform
    return isPlatform("darwin") and platform.processor() == 'powerpc'

# Assume 10.6 and up
def isOSX():
    return isPlatform("darwin") and not isOSX104()

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

# Build a cpp file from a peg definition
def pegBuilder(environment):
    from SCons.Builder import Builder
    from SCons.Action import Action
    return Builder(action = Action(peg_to_cpp, environment['PEG_MAKE']),
                   suffix = '.cpp',
                   src_suffix = '.peg')

def readExec(program):
    import os
    try:
        return os.popen(program).readline().replace("\n",'')
    except OSError:
        return ""

# Try to execute a script that will produce some compiler flags but fail
# gracefully if the script dies or can't be found
def safeParseConfig(environment, config):
    # redirects stderr, not super safe
    def version1():
        import sys
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

# Create a function that pulls out some key from the shell environment
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

useGch = makeUseArgument('gch', True)
usePrx = makeUseEnvironment('prx', False)
isVerbose = makeUseArgument('verbose', False)
useIntel = makeUseEnvironment('intel', False)
useMinpspw = makeUseEnvironment('minpspw', False)
useAndroid = makeUseEnvironment('android', False)
useAndroidX86 = makeUseEnvironment('androidx86', False)
useIos = makeUseEnvironment('ios', False)
usePs3 = makeUseEnvironment('ps3', False)
useNDS = makeUseEnvironment('nds', False)
useDingoo = makeUseEnvironment('dingoo', False)
useXenon = makeUseEnvironment('xenon', False)
usePandora = makeUseEnvironment('pandora', False)
useWii = makeUseEnvironment('wii', False)
useLLVM = makeUseEnvironment('llvm', False)
useNacl = makeUseEnvironment('nacl', False)
useMpg123 = makeUseEnvironment('mpg123', False)
useMad = makeUseEnvironment('mad', False)
useGCW = makeUseEnvironment('gcw', False)
nativeCompile = makeUseEnvironment('native', False)
enableProfiled = makeUseEnvironment('PROFILE', False)
showTiming = makeUseEnvironment('timing', False)
useAllegro4 = makeUseEnvironment('allegro4', False)
useWii = makeUseEnvironment('wii', False)

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

    # FIXME: hack to specify android here
    return byEnv() or byArgument() or useAndroid() or useAndroidX86()

def useSDL():
    return not useAllegro() and not useAllegro5()

# Replace standard tool invocations with nice colored text
def lessVerbose(env):
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

def configure_backend(environment, backends, custom_tests):
    config = environment.Configure(custom_tests = custom_tests)

    if not config.CheckCompiler():
        config.Finish()
        raise Exception("No c++ compiler found. Install gcc or clang")

    class OkBackend(Exception):
        pass

    class NoBackend(Exception):
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
        config.Finish()
        raise NoBackend()
    except OkBackend:
        pass

    return config.Finish()

def checkCompiler(context):
    context.Message("Checking for a compiler (%s) ... " % context.env['CXX'])
    ok = context.TryCompile("""
      int main(int argc, char ** argv){
        return 0;
      }
    """, ".cpp")
    context.Result(colorResult(ok))
    return ok

def detectCPUs():
    import os
    """
    Detects the number of CPUs on a system. Cribbed from pp.
    """

# Linux, Unix and MacOS:
    if hasattr(os, "sysconf"):
        if "SC_NPROCESSORS_ONLN" in os.sysconf_names:
            # Linux & Unix:
            ncpus = os.sysconf("SC_NPROCESSORS_ONLN")
            if isinstance(ncpus, int) and ncpus > 0:
                return ncpus
        else: # OSX:
            return int(os.popen2("sysctl -n hw.ncpu")[1].read())
    # Windows:
    if "NUMBER_OF_PROCESSORS" in os.environ:
        ncpus = int(os.environ["NUMBER_OF_PROCESSORS"]);
        if ncpus > 0:
            return ncpus
    return 1 # Default

