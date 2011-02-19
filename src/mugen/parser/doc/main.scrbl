#lang scribble/manual
@(require scribble/basic scribble/base)

@(title #:version "0.1" "Vembyr Peg Generator Manual")
@author+email["Jon Rafkind" "jon@rafkind.com"]
@(table-of-contents)

@section{Introduction}

Vembyr generates programs that use the Parsing Expression Grammar formalism.
Input to Vembyr is specified by a BNF-like syntax. Currently Vembyr can generate
C++, Ruby, and Python programs with C++ being the most optimized.

Vembyr is implemented as a single python script, @tt{peg.py}.

@section{Command line usage}

Running @tt{peg.py} will produce the following help screen
@verbatim{
Options:
-h,--help,help : Print this help
--help-syntax : Explain syntax of BNF (Backus-Naur form) for grammar files
--bnf : Generate BNF description (grammar language)
--ruby : Generate Ruby parser
--python : Generate Python parser
--cpp,--c++ : Generate C++ parser
--save=filename : Save all generated parser output to a file, 'filename'
--peg-name=name : Name the peg module 'name'. The intermediate peg module will
be written as peg_<name>.py. Defaults to 'peg'.
}

Giving a syntactically correct input file as an argument will result in a
message that says everything is ok.
@verbatim{
$ ./peg.py sample
Grammar file 'sample' looks good!. Use some options to generate a peg parser.
-h will list all available options.
}

An input file with an error in it will result in a parse error.
@verbatim{
$ ./peg.py bad
Read up till line 10, column 1
'Mugen.Def
incl ude: '
          ^
Uh oh, couldn't parse def.peg. Are you sure its using BNF format?
}

Use one of @tt{--cpp}, @tt{--ruby}, @tt{--python} to generate code from the
input specification. By default the output program will be printed to standard
out. Normal shell redirection can be used to put the output into a file or the
@tt{--save=file} option can be used.

@section{Input}
@section{Sample Output}
