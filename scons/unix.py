# SCons build file for Linux

import os
import utils
from SCons.Environment import Environment
from SCons.Script import Exit

def checkCompiler(context):
    context.Message("Checking for a compiler (%s) ... " % context.env['CXX'])
    ok = context.TryCompile("""
      int main(int argc, char ** argv){
        return 0;
      }
    """, ".cpp")
    context.Result(utils.colorResult(ok))
    return ok

def configure(environment):
    custom_tests = {"CheckCompiler": checkCompiler}
    config = environment.Configure(custom_tests = custom_tests)

    if not config.CheckCompiler():
        print("No c++ compiler found. Install gcc or clang")
        Exit(1)

    Exit(0)

def getEnvironment():
    environment = Environment(ENV = os.environ)
    return environment
