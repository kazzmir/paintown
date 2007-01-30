import os;

env = Environment( ENV = os.environ );
config = env.Configure();

flags = [ '-Wall', '-fno-rtti', '-Woverloaded-virtual', '-O' ];
env.Append( CCFLAGS = flags, CPPPATH = [ "." ] );

env.Append( LIBS = [ 'fl', 'ldpng', 'pthread' ] );
env.ParseConfig( 'libpng-config --libs' );
env.ParseConfig( 'allegro-config --libs' );

SConscript( 'src/SConstruct', build_dir='build', exports = 'env' );
env.Install( '.', 'build/paintown' );
env.Install( '.', 'build/test' )
