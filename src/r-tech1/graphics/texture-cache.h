#pragma once

#include "r-tech1/parameter.h"
#include <memory>
#include <functional>
#include <map>
#include "bitmap.h"

namespace Graphics {

class TextureCache{
public:
    static Util::Parameter<std::shared_ptr<TextureCache>> cache;

    TextureCache();
    virtual ~TextureCache();

    std::shared_ptr<Graphics::Bitmap> getTexture(const std::string & key, std::function<std::shared_ptr<Graphics::Bitmap>()> create);

private:
    std::map<std::string, std::shared_ptr<Graphics::Bitmap>> textures;
};

class LocalTextureCache {
public:
    LocalTextureCache();

private:
    Util::Parameter<std::shared_ptr<TextureCache>> local;
};

}
