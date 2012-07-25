def read_cmake_list(name):
    """
    Read a cmake files list and return a dictionary with each cmake variable
    matched to a list of filenames.

    This makes it easy to add/remove files, as only the cmake list needs to be
    modified and scons will automatically pick up the changes.
    """
    lists = {}
    current = []
    reading = False
    for line in open(name):
        if line.startswith("set("):
            current = []
            name = line[4:].strip()
            lists[name] = current
            reading = True
        else:
            if reading:
                # Stop reading files once we hit a line like 'file.cpp)'
                if line.strip("\t\r\n").endswith(")"):
                    reading = False
                path = line.strip("( )\t\r\n")
                if path:
                    current.append(path)
    return lists

def findDirectory(name):
    import os.path
    where = '.'
    # just try 5 directories
    for i in xrange(0, 6):
        if os.path.exists("%s/%s" % (where, name)):
            return "%s/%s" % (where, name)
        where = os.path.join(where, '..')
    raise Exception("Could not find the %s directory" % name)
