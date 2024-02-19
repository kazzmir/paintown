from SCons.Environment import Environment

def getEnvironment():
    import utils

    if utils.useWii():
        import wii
        return wii.getEnvironment()

    if utils.isLinux():
        import unix
        return unix.getEnvironment()
    
    print "Could not determine environment. Defaulting to unix"
    import unix
    return unix.getEnvironment()

def getStaticEnvironment():
    import utils
    if utils.isOSX():
        import osx
        return osx.getStaticEnvironment()

    raise Exception("Could not get static environment")

def buildType(platform):
    platform.sort()
    return '-'.join(platform)

try:
    environment = getEnvironment()
    use = environment
    environment['PAINTOWN_BUILD_TESTS'] = True
    shared = SConscript('src/SConscript', variant_dir = 'build/%s' % buildType(environment['PAINTOWN_PLATFORM']), exports = ['use'] );

    for good in shared:
        safe = environment.Install('.', good)
        environment.Default(safe)
except Exception, e:
    print e

try:
    environment = getStaticEnvironment()
    use = environment
    environment['PAINTOWN_BUILD_TESTS'] = True
    static = SConscript('src/SConscript', variant_dir = 'build/static/%s' % buildType(environment['PAINTOWN_PLATFORM']), exports = ['use'] );

    for good in static:
        safe = environment.InstallAs(good[0].name + '-static', good)
        environment.Alias('static', safe)
except Exception, e:
    print e
