#include "texture-cache.h"
#include "graphics/bitmap.h"

namespace Graphics {

Util::Parameter<std::shared_ptr<TextureCache>> TextureCache::cache;

TextureCache::TextureCache(){
}

TextureCache::~TextureCache(){
}

std::shared_ptr<Graphics::Bitmap> TextureCache::getTexture(const std::string & key, std::function<std::shared_ptr<Graphics::Bitmap>()> create){

    std::map<std::string, std::shared_ptr<Graphics::Bitmap>>::iterator it = textures.find(key);
    if (it == textures.end()){
        std::shared_ptr<Graphics::Bitmap> bitmap = create();
        textures[key] = bitmap;
        return bitmap;
    }

    return it->second;
}

LocalTextureCache::LocalTextureCache():
local(TextureCache::cache, std::shared_ptr<TextureCache>(new TextureCache())){
}

}
