#lang scribble/manual
@(require scribble/basic scribble/base)

@(title #:version "0.1" "Vembyr Peg Generator Manual")
@author+email["Jon Rafkind" "jon@rafkind.com"]
@(table-of-contents)

@section{Introduction}

Vembyr generates programs that use the Parsing Expression Grammar formalism.
Input to Vembyr is specified by a BNF-like syntax. Currently Vembyr can generate
C++, Ruby, and Python programs with C++ being the most optimized.

@section{Files}
@bold{peg.py} The main executable file.@linebreak{}
@bold{cpp_generator.py} Generates c++ parsers.@linebreak{}
@bold{python_generator.py} Generates python parsers.@linebreak{}
@bold{ruby_generator.py} Generates ruby parsers@linebreak{}
@bold{cpp_interpreter_generator.py} Generates c++ parsers that use an
interpreter style.@linebreak{}
@bold{core.py} Contains miscellaneous functions and classes that all other files
require.

@section{Command line usage}

Running @tt{peg.py} will produce the following help screen

Options: @linebreak{}
@italic{-h}, @italic{--help}, @italic{help} : Print this help @linebreak{}
@italic{--help-syntax} : Explain syntax of BNF (Backus-Naur form) for grammar
files @linebreak{}
@italic{--bnf} : Generate BNF description (grammar language) @linebreak{}
@italic{--ruby} : Generate Ruby parser @linebreak{}
@italic{--python} : Generate Python parser @linebreak{}
@italic{--cpp}, --c++ : Generate C++ parser @linebreak{}
@italic{--save=filename} : Save all generated parser output to a file, 'filename'.
Without this option the output of @tt{peg.py} will be sent to standard out.
@linebreak{}
@italic{--peg-name=name} : Name the peg module 'name'. The intermediate peg module will
be written as peg_<name>.py. Defaults to 'peg'. @linebreak{}

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

Grammar files consist of directives at the top of the file followed by BNF rules.

Example:
@verbatim{
start-symbol: start
include: {{

#include <iostream>
static void got_a(){
    std::cout << "Got an 'aa'!" << std::endl;
}
}}

rules:
start = a* b "\\n"* <eof>
a = "aa" {{
     got_a();
   }}

b = "b"
}

@tt{start-symbol} and @tt{include} are directives whereas @tt{rules} starts the BNF
section.

@subsection{Directives}
Available options:
@italic{start-symbol} : The starting non-terminal to use when parsing
starts.@linebreak{}
@hspace[4] @verbatim{start-symbol: top}

@italic{options}: A list of options that modify the behavior of code
generation.@linebreak{}
@tt{debug0} - Disable all debugging output when the PEG runs.@linebreak{}
@tt{debug1} - Enable some debugging.@linebreak{}
@tt{debug2} - Enable even more debugging.@linebreak{}
@tt{no-memo} - Disable the use of memoization completely.@linebreak{}
@hspace[4] @verbatim{options: debug0, no-memo}

@italic{module}: Puts all the generated code into a form that physically
encapsulates it. For C++ this means namespaces, for Ruby this means the
@tt{module} keyword. In C++ the . is converted into nested namespaces so
@tt{Foo.Bar} would become @tt{namespace Foo{ namespace Bar{ ... } }}.
@hspace[4] @verbatim{module: Mugen.Def}.

@italic{include}: Adds arbitrary text to the top of the file outside the
any namespaces that might exist. This is useful for adding C++ #include
directives. Use {{ and }} to delimit the text.

@hspace[4] @verbatim{include: {{
#include <string>
#include <vector>
}}
}

@italic{code}: Add arbitrary text that will appear inside any namespaces that
might exist. This is useful for writing helper methods.
@hspace[4] @verbatim{code: {{
char * get(){
  return "test";
}
}}
}

@subsection{BNF Syntax}

The BNF section starts with a @italic{rules} directive and all the following
text is parsed as BNF syntax. There is no significance to the order of the
rules.

A rule is given by a name followed by an = character and some clauses.

@verbatim{
rules:
    top = "top"
}

Alternatives can be put on a new line preceded by the | symbol.

@verbatim{
top = "top"
    | "bottom"
}

Actions can be given after the clause by writing code inside {{ }} enclosers.

@verbatim{
top = "top" {{ printf("got top!\n"); }}
}

Pattern modifiers can be attached@linebreak{}
@italic{*} --- repeat 0 or more times@linebreak{}
@italic{+} --- repeat 1 or more times@linebreak{}
@italic{?} --- match 0 or 1@linebreak{}

@verbatim{
top = "top"* "bottom"?
}

A plain identifier will call out to another rule.

@verbatim{
top = "top" bottom
bottom = "bottom"
}

The results of a pattern can be stored in a variable by prefixing the name of an
identifier followed by @tt{:}.

@verbatim{
top = what:"top" {{ use(what); }}
}

The type of the variable @tt{what} is @tt{Value} which has the following methods
on it.

@italic{getValue(): void*} --- Get's the underlying object the pattern computed
in its action.@linebreak{}
@italic{getValues(): vector<Value>} --- Gets a list of vector objects when * or
+ is used.

Matched patterns can also be accessed through the $ variables.
@verbatim{
top = "top" bottom "another" {{ use($1); // use "top"
                                use($2); // use bottom
                                use($3); // use "another
                             }}
}

Literal strings can be followed by @bold{{case}}. {case} does a case insensitive
match on the string.
@verbatim{
match_foo = "foo"{case}
}

Will match "foo", "foO", "FOO", or any other variation on "foo" with upper case
letters.

Special patterns exist for specific circumstances.
@itemize{
  @item{@bold{<eof>} parses when the end of input is reached.}
  @item{@bold{<ascii #>} parses a character with the given ascii code for when
  you need to parse a character with an unprintable character (such as any
  character above 128). Put a number where the # goes, anything from 0 to 255.}
  @item{@bold{<void>} parses nothing.}
  @item{@bold{<line>} parses nothing but returns an object that contains
  information about the current source position. Use the methods
  @bold{getCurrentLine} and @bold{getCurrentColumn} on this object.
  @verbatim{
  stuff = source:<line> "ok" {{ printf("current line %d column %d\n",
  getCurrentLine(source), getCurrentColumn(source)); }}
  }}
}

@section{Sample Output}
