import os;

def isWindows():
	import re
	import sys
	return "win32" in sys.platform

def isOSX():
	import re
	import sys
	return "darwin" in sys.platform

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

def getEnvironment():
    if useMingw():
	return Environment( ENV = os.environ, tools = ['mingw'] )
    else:
	return Environment( ENV = os.environ )

if isWindows():
    print "Try 'scons env=mingw' if you want to use mingw's gcc instead of visual studio or borland"
    
env = getEnvironment()

def getDebug():
	try:
		return int(os.environ[ 'DEBUG' ])
	except KeyError:
		return 0

debug = getDebug()

cflags = [ '-Wall', '-Werror' ]
cppflags = [ '-fno-rtti', '-Woverloaded-virtual' ]
cdefines = []
# cppflags = [ '-Woverloaded-virtual' ]

if debug:
	cflags.append( ['-g3','-ggdb'] )
else:
	cflags.append( '-O2' )

# env.Append( CCFLAGS = '-m32' )
# env.Append( LINKFLAGS = '-m32' )

# env.Append( CCFLAGS = cflags, CXXFLAGS = cppflags, CPPPATH = [ ".", 'sockets' ] )
env.Append( CCFLAGS = cflags, CXXFLAGS = cppflags, CPPPATH = [ ".", 'hawknl' ], CPPDEFINES = cdefines )

def buildDumb(where, env):
	return SConscript( "src/dumb/SConscript", build_dir = '%s/dumb' % where, exports = 'env' )

def buildHawknl(where, env):
	return SConscript( "src/hawknl/SConscript", build_dir = '%s/hawknl' % where, exports = 'env' )

dumbEnv = Environment( ENV = os.environ )
hawkEnv = Environment( ENV = os.environ )
dumbStaticEnv = Environment( ENV = os.environ )
hawkStaticEnv = Environment( ENV = os.environ )

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

if False:
	env.Append( CCFLAGS = '-pg' )
	env.Append( LINKFLAGS = '-pg' )

staticEnv = env.Copy()

# env.Append( LIBS = [ 'aldmb', 'dumb' ] );
env.Append( LIBS = [dumb,hawknl] )


if isWindows():
	env.Append( LIBS = [ 'alleg', 'pthreadGC2', 'png', 'freetype', 'z', 'wsock32' ] )
	env.Append( CPPDEFINES = 'WINDOWS' )
	env.Append( CCFLAGS = ['-mwindows','-mthreads'] )
	env.Append( LINKFLAGS = ['-mwindows','-mthreads'] )
	staticEnv.Append( LIBS = [ 'alleg', 'pthreadGC2', 'png', 'freetype', 'z', 'wsock32' ] )
	staticEnv.Append( CPPDEFINES = 'WINDOWS' )
else:
	env.Append( LIBS = [ 'pthread' ] )
	staticEnv.Append( LIBS = [ 'pthread' ] )

	staticEnv.Append( LIBS = [ hawknl_static, dumb_static ] )

	try:
		dumbStaticEnv.ParseConfig( 'allegro-config --cflags' )
	except OSError:
		pass

	if isOSX():
		staticEnv[ 'CXX' ] = 'misc/g++'
		staticEnv[ 'CC' ] = 'misc/gcc'
	
	config = env.Configure()
	try:
		config.env.ParseConfig( 'allegro-config --libs --cflags' )
		config.env.ParseConfig( 'freetype-config --libs --cflags' )
		config.env.ParseConfig( 'libpng-config --libs --cflags' )
		
		staticEnv.ParseConfig( 'allegro-config --static --libs --cflags' )
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

	if False:
		## why doesnt this work!
		if not config.TryCompile("int main(){ return 0; }\n", ".c"):
			print "You need a C compiler such as gcc installed"
			Exit( 1 )
		if not config.TryCompile("class a{}; int main(){ a x = new a(); delete x; return 0; }\n", ".cpp" ):
			print "You need a C++ compiler such as g++ installed"
			Exit( 1 )
	if not config.CheckHeader( 'allegro.h' ):
		print "You need the header files for Allegro. Get it from http://alleg.sf.net"
		Exit( 1 )
	if not config.CheckHeader( 'ft2build.h' ):
		print "You need freetype. Install freetype and/or X11"
		Exit( 1 )
	if not config.CheckHeader( 'png.h' ):
		print "You need libpng. Get it from http://www.libpng.org/pub/png/libpng.html"
		Exit( 1 )
	env = config.Finish()

use = env
shared = SConscript( 'src/SConstruct', build_dir='build', exports = 'use' );

use = staticEnv
static = SConscript( 'src/SConstruct', build_dir='build-static', exports = 'use' )

for i in shared:
	Default(env.Install( '.', i ))

for i in static:
	Alias('static',env.InstallAs( i[0].name + '-static', i ))

# if False:
# 	if isWindows():
# 		env.Install( '.', 'build/paintown.exe' )
# 		env.Install( '.', 'build/test.exe' )
# 	else:
# 		env.Install( '.', 'build/paintown' )
# 		env.Install( '.', 'build/test' )
