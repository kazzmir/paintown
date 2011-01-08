#lang scribble/manual
@(require scribble/basic)

@(title #:version "0.1" "Paintown Manual")
@author+email["Jon Rafkind" "jon@rafkind.com"]
@(table-of-contents)

@;@include-section["design.scrbl"]
@section{Menu}
Adventure mode
Adventure mode with cpu
Mugen Mode
Change Mod
Network mode - Host
Network mode - Join
@subsection{Options}

@section{Mugen}
Paintown supports an implementation of the M.U.G.E.N engine originally developed
by Elecbyte @url{http://www.elecbyte.com}. Our goal is to at least support a
majority of the content created for M.U.G.E.N version 2002.04.14 and eventually
content created for version M.U.G.E.N 1.0.

The code for the M.U.G.E.N implementation is mostly self-contained in the
@bold{mugen} sub-directory of @bold{src}.

@;TODO: insert screenshots of the original mugen and paintown's implementation here

@subsection{Architecture}

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
