import os;

env = Environment( ENV = os.environ );
config = env.Configure();

def isWindows():
	import re
	import sys
	return "win32" in sys.platform

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

env.Append( LIBS = [ 'ldpng', 'aldmb', 'dumb' ] );
if isWindows():
	env.Append( LIBS = [ 'alleg', 'pthreadGC2', 'png', 'freetype' ] )
	env.Append( CPPDEFINES = 'WINDOWS' )
else:
	env.Append( LIBS = [ 'pthread' ] )
	env.ParseConfig( 'libpng-config --libs --cflags' );
	env.ParseConfig( 'allegro-config --libs --cflags' );
	env.ParseConfig( 'freetype-config --libs --cflags' );

SConscript( 'src/SConstruct', build_dir='build', exports = 'env' );
if isWindows():
	env.Install( '.', 'build/paintown.exe' )
	env.Install( '.', 'build/test.exe' )
else:
	env.Install( '.', 'build/paintown' )
	env.Install( '.', 'build/test' )

# editorEnv = Environment()
# 
# editorEnv.BuildDir( 'build-editor', 'src' )
# editorEnv.Append( CPPPATH = ['saggui/include'] )
# editorEnv.ParseConfig( 'allegro-config --libs' )
# editorEnv.Append( LIBS = ['saggui-alleg','saggui'] )
# editorEnv.Append( LIBPATH = 'saggui/lib' )
# editorEnv.Program( 'level-editor', 'build-editor/editor/editor.cpp' )
