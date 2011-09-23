# build for the iphone/ipad

# A lot of code here is just copy/pasted from the main SConstruct file.
# Refactor this all out.

from SCons.Environment import Environment
from SCons.Script import Exit
import os

def isPlatform(platform):
    import sys
    return platform in sys.platform

def isWindows():
    return isPlatform("win32")

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

def dummyCheck(context):
    context.Result(1)
    return 1

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

def iphoneEnvironment():
    base = Environment(ENV = os.environ)
    gccversion = '4.2'
    iostarget = '4.3'
    osxtarget = '10.5'
    platform_phone = 'iPhoneOS'
    # fill the rest in ...

def simulatorEnvironment():
    base = Environment(ENV = os.environ)
    gccversion = '4.2'
    iostarget = '4.3'
    osxtarget = '10.6'
    platform_sim = 'iPhoneSimulator'
    arch_sim = 'i686'
    sim_bin_dir = '/Developer/Platforms/%s.platform/Developer/usr/bin' % platform_sim
    base.PrependENVPath('PATH', sim_bin_dir)
    sdkroot_sim = '/Developer/Platforms/%s.platform/Developer/SDKs/%s%s.sdk' % (platform_sim, platform_sim, iostarget)
    postfix = '-%s' % gccversion
    def setup(x, post):
        return '%s%s' % (x, post)
    # only the compiler can have different versions
    base['CC'] = setup('gcc', postfix)
    base['CXX'] = setup('g++', postfix)

    def stringify(array):
        str = ''
        for item in array:
            str += item + ' '
        return [str.split(" ")]

    cflags_sim = ['-std=gnu99', '-fobjc-legacy-dispatch', '-fobjc-abi-version=2']
    ccflags_sim = ['-arch %s' % arch_sim, '-pipe', '-mdynamic-no-pic', '-fvisibility=hidden', '-isysroot %s' % sdkroot_sim, '-mmacosx-version-min=%s' % osxtarget] + Split("""-g -O2 -gdwarf-2 -mthumb -Wall -Wmissing-prototypes -ffast-math -fno-strict-aliasing -fmessage-length=0 -pipe -Wno-trigraphs -fpascal-strings -Wmost -Wno-four-char-constants -Wno-unknown-pragmas -gdwarf-2 -Wall -fno-strict-aliasing""")
    ldflags_sim = ['-arch %s' % arch_sim, '-isysroot %s' % sdkroot_sim, '-Wl,-dead_strip', '-mmacosx-version-min=%s' % osxtarget, '-Xlinker -objc_abi_version', '-Xlinker 2', '-Wl,-search_paths_first', '-Wl,-headerpad_max_install_names']
    frameworks = Split("""OpenGLES CoreGraphics QuartzCore UIKit Foundation CoreFoundation OpenAL""")

    # ldflags_sim = ['-arch %s' % arch_sim, '-framework OpenGLES']
    defines_sim = Split("""__IPHONE_OS_VERSION_MIN_REQUIRED=40300""")
    cppflags = Split("")

    def setFlags(includes, cflags, ccflags, cppflags, ldflags, defines, env):
        env.Append(CPATH = includes)
        env.Append(CPPPATH = includes)
        env.Append(CFLAGS = cflags)
        env.Append(CCFLAGS = ccflags)
        env.Append(CPPFLAGS = cppflags)
        env.Append(LINKFLAGS = ldflags)
        env.Append(CPPDEFINES = defines)
        
    # base.Append(LIBS = ['allegro-sim'])
    base.Append(FRAMEWORKS = frameworks)
    base['LIBS'] = ['allegro', 'allegro_acodec', 'allegro_audio', 'allegro_image', 'allegro_memfile', 'allegro_primitives', 'allegro_ttf', 'allegro_font']
    base.ParseConfig('tmp/freetype-sim/bin/freetype-config --cflags --libs')
    base.Append(LIBPATH = ['#misc/allegro-iphone/install/sim/lib'])
    includes = [sdkroot_sim]
    #  + ['%s/usr/include/c++/4.2.1' % sdkroot_sim] + ['%s/usr/include/c++/4.2.1/i686-apple-darwin10' % sdkroot_sim]
    setFlags(includes + ['%s/usr/include' % sdkroot_sim], stringify(cflags_sim), stringify(ccflags_sim), cppflags, stringify(ldflags_sim), defines_sim, base)

    return base

env = simulatorEnvironment()
env['PEG_MAKE'] = 'Creating peg parser $TARGET'

buildDir = 'build-allegro5-iphone'

# Generates a single .cpp file from a .peg description
peg_builder = Builder(action = Action(peg_to_cpp, env['PEG_MAKE']),
                      suffix = '.cpp',
                      src_suffix = '.peg')

env.Append(BUILDERS = {'Peg' : peg_builder})
env['PAINTOWN_NETWORKING'] = False
env['PAINTOWN_BACKEND'] = 'allegro5'
env['PAINTOWN_BUILD_TESTS'] = False
env['PAINTOWN_TESTS'] = {'CheckPython': dummyCheck}
env['PAINTOWN_COLORIZE'] = colorize
env['PAINTOWN_USE_PRX'] = False
env.Append(CPPPATH = ["#%s" % buildDir, '#misc/allegro-iphone/install/sim/include'])
env.Append(CPPDEFINES = ['USE_ALLEGRO5', 'IPHONE'])
use = env
shared = SConscript('src/SConscript', variant_dir = buildDir, exports = ['use'] );
