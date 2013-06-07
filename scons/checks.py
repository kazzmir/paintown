import utils

rtti_counter = 0
def checkRTTI(context):
    global rtti_counter
    rtti_counter += 1
    context.Message("Checking if we need rtti... ")
    tmp = context.env.Clone()
    env = context.env
    env.Append(CXXFLAGS = ['-fno-rtti'])

    ret = context.TryCompile("""
      #include <exception>
      int main(int argc, char ** argv){
        extern void foo();
        try{
          foo();
        } catch (const std::exception & e){
          return 1;
        }
        return 0;
      }
    """, ".cpp")

    s1 = context.lastTarget

    ret = context.TryCompile("""
      #include <exception>
      void foo(){
        throw std::exception();
      }
    """, ".cpp")

    s2 = context.lastTarget

    result = None
    spawn = context.sconf.env['SPAWN']
    try:
        context.sconf.env['SPAWN'] = context.sconf.pspawn_wrapper
        nodes = env.Program(context.sconf.confdir.File('rtti%d' % rtti_counter), [s1,s2])
        result = context.sconf.BuildNodes(nodes)
    except Exception:
        result = False

    context.sconf.env['SPAWN'] = spawn

    foo = 0

    if not result:
        context.sconf.env = tmp
        foo = 1

    context.Result(utils.colorResult(foo))
    return foo

def checkAllegro5(debug):
    use_debug = [""]
    if debug:
        use_debug[0] = "-debug"
    def make(context):
        context.Message("Checking for Allegro 5 ... ")
        tmp = context.env.Clone()
        env = context.env
        def find(version):
            context.Message(str(version))
            try:
                def make(name):
                    return '%s%s-%s' % (name, use_debug[0], version)
                libraries = [make('allegro'),
                             make('allegro_ttf'),
                             make('allegro_memfile'),
                             make('allegro_image'),
                             make('allegro_primitives'),
                             make('allegro_audio'),
                             make('allegro_acodec')]
                utils.safeParseConfig(env, 'pkg-config %s --cflags --libs' % ' '.join(libraries))
                env.Append(CPPDEFINES = ['USE_ALLEGRO5'])
                context.Message('found version %s' % version)
                return True
            except Exception, e:
                print e
                return False
        try:
            ok = 0
            # if find(5.1) or find(5.0):
            if find(5):
                ok = 1
            context.Result(utils.colorResult(ok))
            return ok
        except:
            context.sconf.env = tmp
        context.Result(utils.colorResult(0))
        return 0

    return make

