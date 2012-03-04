from SCons.Environment import Environment

def getEnvironment():
    import utils
    if utils.isLinux():
        import unix
        return unix.getEnvironment()
    
    print "Could not determine environment. Defaulting to unix"
    import unix
    return unix.getEnvironment()

def buildType(platform):
    return '-'.join(platform)

environment = getEnvironment()
use = environment
environment['PAINTOWN_BUILD_TESTS'] = True
shared = SConscript('src/SConscript', variant_dir = 'build/%s' % buildType(environment['PAINTOWN_PLATFORM']), exports = ['use'] );

for good in shared:
    safe = environment.Install('.', good)
    environment.Default(safe)
