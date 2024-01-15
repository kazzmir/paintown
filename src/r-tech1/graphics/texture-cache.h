#pragma once

#include "parameter.h"
#include <memory>
#include <functional>

namespace Graphics {

class TextureCache{
public:
    TextureCache();
    virtual ~TextureCache();

    std::shared_ptr<Graphics::Bitmap> getTexture(const std::string & key, std::function<std::shared_ptr<Graphics::Bitmap>()> create);

private:
    std::map<std::string, std::shared_ptr<Graphics::Bitmap>> textures;
};

}
