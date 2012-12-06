#lang scribble/manual

@title{Implementation}

@section{Libraries}
@subsection{Internal}
DUMB @url{http://dumb.sf.net} Plays MOD/S3M/XM/IT formatted music files
SDL Image Loads various image file formats (png)
SDL Mixer Software mixer for SDL
Sprig Provides drawing and blending routines for SDL
gme Plays various console game formats (nintendo - nsf, sega - gym)
hawknl Network abstraction library

@subsection{External}
libsdl Provides graphics and input abstractions over various OS's
allegro Provides graphics and input abstractions over various OS's
libpng Required to load png files
libz Required by libpng
libfreetype Loads true type fonts
