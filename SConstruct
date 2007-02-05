import os;

env = Environment( ENV = os.environ );
config = env.Configure();

debug = 1

flags = [ '-Wall', '-Werror', '-fno-rtti', '-Woverloaded-virtual' ];

if debug:
	flags.append( '-g3' )
else:
	flags.append( '-O2' )

env.Append( CCFLAGS = flags, CPPPATH = [ "." ] );

env.Append( LIBS = [ 'fl', 'ldpng', 'pthread', 'aldmb', 'dumb' ] );
env.ParseConfig( 'libpng-config --libs' );
env.ParseConfig( 'allegro-config --libs' );

SConscript( 'src/SConstruct', build_dir='build', exports = 'env' );
env.Install( '.', 'build/paintown' );
env.Install( '.', 'build/test' )
