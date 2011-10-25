def ps3(env):
        print "Environment is ps3 (ps3 development)"
        # symlink the ps3dev to /opt/ps3dev, or just put it there
        # Needs these environment variables
        # export PS3DEV=/opt/ps3dev
        # export PATH=$PATH:$PS3DEV/bin
        # export PATH=$PATH:$PS3DEV/ppu/bin
        # export PATH=$PATH:$PS3DEV/spu/bin
        # export PSL1GHT=$PS3DEV/psl1ght
        # export PATH=$PATH:$PSL1GHT/bin
        path = ps3devPath()
        bin_path = path + '/ppu/bin/'
        prefix = 'ppu-'
        def setup(pre, x):
            return '%s%s' % (pre, x)
        env['CC'] = setup(prefix, 'gcc')
        env['LD'] = setup(prefix, 'ld')
        env['CXX'] = setup(prefix, 'g++')
        env['AS'] = setup(prefix, 'as')
        env['AR'] = setup(prefix, 'ar')
        env['OBJCOPY'] = setup(prefix, 'objcopy')
        # FIXME: try to use sdl-config to find these paths
        # instead of hard coding them
        safeParseConfig(env, path + '/portlibs/ppu/bin/freetype-config --cflags --libs')
        safeParseConfig(env, path + '/portlibs/ppu/bin/libpng-config --cflags --libs')

        # FIXME: it uses -lc-glue-ppu which I can't find maybe I missed something in the setup for now I'll put it down below
        #env.ParseConfig(bin_path +'sdl-config --cflags --libs') 
        
        env.Append(CPPPATH = [setup(path, "/ppu/include"),
                              setup(path, "/spu/include"),
                              setup(path, "/portlibs/ppu/include"),
                              setup(path, "/portlibs/ppu/include/SDL"),
                              setup(path, "/portlibs/ppu/include/vorbis"),
                              setup(path, "/portlibs/ppu/include/ogg"),
                              setup(path, "/psl1ght/ppu/include"),
                              setup(path, "/psl1ght/spu/include")])
        env.Append(CPPDEFINES = ['PS3'])
        env.Append(LIBPATH = [setup(path, '/ppu/lib'),
                              setup(path, '/spu/lib'),
                              setup(path, '/portlibs/ppu/lib'),
                              setup(path, '/psl1ght/ppu/lib'),
                              setup(path, '/psl1ght/spu/lib')])
        # -mminimal-toc puts less stuff in the table of contents "TOC".
        # I don't know why this is important for the ps3 compiler but apparently
        # paintown was overflowing the TOC causing it to crash on startup.
        flags = ['-mcpu=cell', '-mhard-float', '-fmodulo-sched', '-ffunction-sections', '-fdata-sections', '-maltivec', '-mminimal-toc']
        env.Append(CCFLAGS = flags)
        env.Append(CXXFLAGS = flags)
        env['LINKCOM'] = '$CC $LINKFLAGS $SOURCES -Wl,--start-group $_LIBDIRFLAGS $_LIBFLAGS -Wl,--end-group -o $TARGET'
        env.Append(LINKFLAGS = flags)
        # Removed reality and psl1ght
        all = Split("""
SDL
ogg
vorbis
vorbisfile
stdc++
m
gcm_sys
sysutil
lv2
io
net
sysmodule
audio
png
z
jpeg
c
rsx
""")
        env.Append(LIBS = all)
        env.PrependENVPath('PATH', bin_path)
        env.PrependENVPath('PKG_CONFIG_PATH', path + '/portlibs/ppu/lib/pkgconfig')
        return env

