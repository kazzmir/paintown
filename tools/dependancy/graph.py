#!/usr/bin/env python

# dependancy will just be a map from a file to a list of files

# true if the directory contains a .svn in it
def svn(dir):
    return '.svn' in dir

def read_includes(file):
    import re
    input = open(file)
    local_include = re.compile('^#include\s+"(.*)"')
    system_include = re.compile('^#include\s+<(.*)>')
    for line in input.readlines():
        out = local_include.match(line)
        if out != None:
            yield out.group(1)
        else:
            out = system_include.match(line)
            if out != None:
                yield out.group(1)
        
    input.close()

def read_dependancies(file, root):
    out = []
    for include in read_includes(file):
        import os
        if os.path.exists(include):
            out.append(include)
        elif os.path.exists(root + '/' + include):
            out.append(os.path.normpath('%s/%s' % (root, include)))
        else:
            out.append(include)
    return out

def find_dependancies(dir):
    import os

    def source(name):
        import re
        c_file = re.compile('.*\.c$')
        cpp_file = re.compile('.*\.cpp$')
        h_file = re.compile('.*\.h$')
        return c_file.match(name) != None or\
               cpp_file.match(name) != None or\
               h_file.match(name) != None

    out = {}
    for root, dirs, files in os.walk(dir):
        if not svn(root):
            for file in [f for f in files if source(f)]:
                print "%s %s" % (root, file)
                out[os.path.normpath('%s/%s' % (root,file))] = read_dependancies('%s/%s' % (root, file), root)

    return out

def generate_graph(dependancies):

    data = []

    names = {}
    next_name = [0]
    def dot_name(name):
        try:
            return names[name]
        except KeyError:
            next_name[0] = next_name[0] + 1
            names[name] = 's%s' % next_name[0]
            return names[name]

    def addLink(from_file, to_file):
        # memoize these names so they aren't repeatedly added
        data.append('%s [label = "%s"]' % (dot_name(from_file), from_file))
        data.append('%s [label = "%s"]' % (dot_name(to_file), to_file))
        data.append("%s -> %s;" % (dot_name(from_file), dot_name(to_file)))

    for key, value in dependancies.items():
        for to in value:
            addLink(key, to)

    return """
digraph X {
%s
}
""" % '\n'.join(data)

# generates a ps file
def run_dot(file):
    import subprocess
    out = "%s.ps" % file
    subprocess.call(['dot', '-Tps', file, '-o', out])
    print "Generated %s" % out

def generate(dir, out):
    dependancies = find_dependancies(dir)
    fout = open(out, 'w')
    fout.write(generate_graph(dependancies))
    fout.close()
    run_dot(out)

# make these command line parameters
generate('.', 'out')
