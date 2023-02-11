#ifndef _rtech1_resource_manager_h
#define _rtech1_resource_manager_h

#include "file-system.h"
#include "thread.h"
#include "pointer.h"
#include <map>
#include <string>
#include <vector>

class Sound;
namespace Graphics{
class Bitmap;
}

/* thread-safe */
class Resource{
public:
    /* do not prepend Util::getDataPath() to paths. Resource will do it for you.
     */
    static Util::ReferenceCount<Sound> getSound(const Filesystem::RelativePath & path);
    static Util::ReferenceCount<Graphics::Bitmap> getBitmap(const Filesystem::RelativePath & path);
    static void destroy();
    static void initialize();

private:
    static Resource * getResource();

    Resource();
    virtual ~Resource();

    Util::ReferenceCount<Sound> _getSound(const Filesystem::AbsolutePath & path);
    Util::ReferenceCount<Graphics::Bitmap> _getBitmap(const Filesystem::AbsolutePath & path);

private:
    static Resource * resource;
    std::map<std::string, Util::ReferenceCount<Sound> > sounds;
    std::map<std::string, Util::ReferenceCount<Graphics::Bitmap> > bitmaps;
    Util::Thread::LockObject lock;
};

#endif
