# http://codeliberates.blogspot.com/2008/05/detecting-cpuscores-in-python.html
def detectCPUs():
    import os
    """
    Detects the number of CPUs on a system. Cribbed from pp.
    """

# Linux, Unix and MacOS:
    if hasattr(os, "sysconf"):
        if os.sysconf_names.has_key("SC_NPROCESSORS_ONLN"):
            # Linux & Unix:
            ncpus = os.sysconf("SC_NPROCESSORS_ONLN")
            if isinstance(ncpus, int) and ncpus > 0:
                return ncpus
        else: # OSX:
            return int(os.popen2("sysctl -n hw.ncpu")[1].read())
    # Windows:
    if os.environ.has_key("NUMBER_OF_PROCESSORS"):
        ncpus = int(os.environ["NUMBER_OF_PROCESSORS"]);
        if ncpus > 0:
            return ncpus
    return 1 # Default

def usingIcecream():
    import os
    try:
        return int(os.environ['USE_ICECC']) == 1
    except KeyError:
        return False

# if using the icecc distributed compiler stuff just set cores to 5
if usingIcecream():
    print("5")
else:
    print(detectCPUs())
