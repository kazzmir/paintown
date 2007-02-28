import os;

env = Environment( ENV = os.environ );
config = env.Configure();

def getDebug():
	try:
		return os.environ[ 'DEBUG' ]
	except KeyError:
		return 0

debug = getDebug()

flags = [ '-Wall', '-Werror', '-fno-rtti', '-Woverloaded-virtual' ];

if debug:
	flags.append( '-g3' )
else:
	flags.append( '-O2' )

env.Append( CCFLAGS = flags, CPPPATH = [ "." ] );

env.Append( LIBS = [ 'fl', 'ldpng', 'pthread', 'aldmb', 'dumb' ] );
env.ParseConfig( 'libpng-config --libs' );
env.ParseConfig( 'allegro-config --libs' );
env.ParseConfig( 'freetype-config --libs --cflags' );

SConscript( 'src/SConstruct', build_dir='build', exports = 'env' );
env.Install( '.', 'build/paintown' );
env.Install( '.', 'build/test' )
