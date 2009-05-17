"""tools.gch

Quick tool for creating precompiled headers for gcc

This is a very trimmed down copy SCons.Tool.qt to get a
simple gchBuilder

-Nathan Letwory (jesterKing)
"""

#
import os.path
import re

import SCons.Defaults
import SCons.Tool
import SCons.Util

header_extensions = [".hxx", ".hpp", ".hh", ".h"]
cplusplus = __import__('c++', globals(), locals(), [])
cxx_suffixes = cplusplus.CXXSuffixes

def find_file(filename, paths, node_factory):
    retval = None
    for dir in paths:
        node = node_factory(filename, dir)
        if node.rexists():
            return node
    return None


class _Autogch:
    """
    Callable class, which works as an emitter for Programs, SharedLibraries and
    StaticLibraries.
    """

    def __init__(self, objBuilderName):
        self.objBuilderName = objBuilderName
        
    def __call__(self, target, source, env):
        """
        'Smart' function. Gets the list of objects for the Program
        or Lib. Adds objects and builders for the special gch files.
        """
        debug = 0
        
        # some shortcuts used in the scanner
        FS = SCons.Node.FS.default_fs
        splitext = SCons.Util.splitext
        objBuilder = getattr(env, self.objBuilderName)
  
        # The following is kind of hacky to get builders working properly (FIXME)
        objBuilderEnv = objBuilder.env
        objBuilder.env = env
        gchBuilderEnv = env.Gch.env
        env.Gch.env = env
        
        # make a deep copy for the result; Gch objects will be appended
        out_sources = source[:]

        for obj in source:
            if not obj.has_builder():
                continue
            cpp = obj.sources[0]
            if not splitext(str(cpp))[1] in cxx_suffixes:
                if debug:
                    print "scons: gch: '%s' is no cxx file. Discarded." % str(cpp) 
                continue
            h=None
            for h_ext in header_extensions:
                # try to find the header file in the corresponding source
                # directory
                hname = splitext(cpp.name)[0] + h_ext
                h = find_file(hname,
                              (cpp.get_dir(),),
                              FS.File)
                if h:
                    if debug:
                        print "scons: gch: Scanning '%s' (header of '%s')" % (str(h), str(cpp))
                    break
            if not h and debug:
                print "scons: gch: no header for '%s'." % (str(cpp))
            if h:
                gch_cpp = env.Gch(h)
                o = env.Object(cpp)
                env.Depends(o,gch_cpp) # we assume we have a file.cpp file.header_extension pair
                                       # as exercise to the reader to improve this
                if debug:
                    print "scons: gch: found '%s', gch'ing to '%s'" % (str(h), str(gch_cpp[0]))
        # restore the original env attributes (FIXME)
        objBuilder.env = objBuilderEnv
        env.Gch.env = gchBuilderEnv

        return (target, out_sources)

AutogchShared = _Autogch('SharedObject')
AutogchStatic = _Autogch('StaticObject')

def generate(env):
    """Add Builder to an Environment"""

    CLVar = SCons.Util.CLVar
    Action = SCons.Action.Action
    Builder = SCons.Builder.Builder
    splitext = SCons.Util.splitext

    env['GCHFROMHCOM'] = (
            '$CXX $_CPPDEFFLAGS $_CPPINCFLAGS -x c++ -c -o ${TARGETS[0]} $SOURCE')

    gchBld = Builder(action={}, prefix={}, suffix={})
    for h in header_extensions:
        gchBld.add_action(h, '$GCHFROMHCOM')
        gchBld.prefix[h] = ''
        gchBld.suffix[h] = h+'.gch'

    env['BUILDERS']['Gch'] = gchBld

    env.AppendUnique(PROGEMITTER =[AutogchStatic],
                     SHLIBEMITTER=[AutogchShared],
                     LIBEMITTER  =[AutogchStatic]
                     )

def exists(env):
    return true

