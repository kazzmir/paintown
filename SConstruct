import os;

env = Environment( ENV = os.environ );
config = env.Configure();

def isWindows():
	import re
	import sys
	return re.compile( '.*win32.*' ).match( sys.platform )

def getDebug():
	try:
		return int(os.environ[ 'DEBUG' ])
	except KeyError:
		return 0

debug = getDebug()

flags = [ '-Wall', '-Werror', '-fno-rtti', '-Woverloaded-virtual' ];

if debug:
	flags.append( '-g3' )
else:
	flags.append( '-O2' )

env.Append( CCFLAGS = flags, CPPPATH = [ "." ] );

if False:
	env.Append( CCFLAGS = '-pg' )
	env.Append( LINKFLAGS = '-pg' )

env.Append( LIBS = [ 'fl', 'ldpng', 'pthread', 'aldmb', 'dumb' ] );
if isWindows():
	env.Append( LIBS = [ 'alleg' ] )
	env.Append( CPPDEFINES = 'WINDOWS' )
else:
	env.ParseConfig( 'libpng-config --libs' );
	env.ParseConfig( 'allegro-config --libs' );
	env.ParseConfig( 'freetype-config --libs --cflags' );

SConscript( 'src/SConstruct', build_dir='build', exports = 'env' );
env.Install( '.', 'build/paintown' );
env.Install( '.', 'build/test' )
