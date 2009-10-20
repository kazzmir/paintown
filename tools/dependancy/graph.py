#!/usr/bin/env python

# dependancy will just be a map from a file to a list of files

def find_dependancies(dir):
    import os
    return None

def generate_graph(dependancies):

    data = []

    def addLink(from_file, to_file):
        data.append("%s -> %s;" % (from_file, to_file))

    addLink('a', 'b')
    addLink('b', 'c')

    return """
digraph X {
%s
}
""" % '\n'.join(data)

def run_dot(file):
    import subprocess
    subprocess.call(['dot', '-Tps', file, '-o', '%s.ps' % file])

def generate(dir, out):
    dependancies = find_dependancies(dir)
    fout = open(out, 'w')
    fout.write(generate_graph(dependancies))
    fout.close()
    run_dot(out)

# make these command line parameters
generate('.', 'out')
