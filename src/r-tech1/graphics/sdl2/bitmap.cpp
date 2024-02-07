#ifdef USE_SDL2

#include "bitmap.h"
#include "../bitmap.h"
#include "../../debug.h"
#include "r-tech1/file-system.h"
#include "r-tech1/init.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <memory>

using namespace std;

/* RAII wrapper for window/renderer */
class SDLGlobalHandler{
public:
    SDL_Window* window;
    SDL_Renderer* renderer;

    SDLGlobalHandler(SDL_Window* window, SDL_Renderer* renderer):
    window(window),
    renderer(renderer){
    }

    ~SDLGlobalHandler(){
        if (renderer != nullptr){
            SDL_DestroyRenderer(renderer);
        }
        if (window != nullptr){
            SDL_DestroyWindow(window);
        }

        renderer = nullptr;
        window = nullptr;
    }
};

unique_ptr<SDLGlobalHandler> global_handler;

bool operator==(const INTERNAL_COLOR& left, const INTERNAL_COLOR& right){
    return left.r == right.r && left.g == right.g && left.b == right.b;
}

bool operator!=(const INTERNAL_COLOR& left, const INTERNAL_COLOR& right){
    return !(left == right);
}

bool operator<(const INTERNAL_COLOR&, const INTERNAL_COLOR&){
    /* FIXME */
    return false;
}

Graphics::Bitmap Graphics::makeRoundedRect(int width, int height, int radius, const Graphics::Color & fillColor, const Graphics::Color & borderColor){

    SDL_Texture* texture = SDL_CreateTexture(global_handler->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetRenderTarget(global_handler->renderer, texture);
    SDL_SetRenderDrawBlendMode(global_handler->renderer, SDL_BLENDMODE_NONE);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(global_handler->renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(global_handler->renderer, nullptr);

    roundedBoxRGBA(global_handler->renderer, 0, 0, width, height, radius, getRed(fillColor), getGreen(fillColor), getBlue(fillColor), 255);
    roundedRectangleRGBA(global_handler->renderer, 0, 0, width-1, height-1, radius, getRed(borderColor), getGreen(borderColor), getBlue(borderColor), 255);

    return Graphics::Bitmap(texture);
}

Graphics::Bitmap::Bitmap(){
    setData(std::shared_ptr<BitmapData>(new BitmapData(nullptr, nullptr)));
}

Graphics::Bitmap::Bitmap(int width, int height){
    if (Global::isMainThread()){
        SDL_Texture* texture = SDL_CreateTexture(global_handler->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        setData(std::shared_ptr<BitmapData>(new BitmapData(nullptr, texture)));
    } else {
        setData(std::shared_ptr<BitmapData>(new BitmapData(SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0), nullptr)));
    }

    clip_x1 = 0;
    clip_y1 = 0;
    clip_x2 = width;
    clip_y2 = height;
}

Graphics::Bitmap::Bitmap( const Bitmap & copy, bool deep_copy ):
clip_x1(copy.clip_x1),
clip_y1(copy.clip_y1),
clip_x2(copy.clip_x2),
clip_y2(copy.clip_y2){
    setData(copy.data);
}

Graphics::Bitmap::Bitmap( const Bitmap & copy, int x, int y, int width, int height ){
    setData(copy.data);

    clip_x1 = copy.clip_x1 + x;
    clip_y1 = copy.clip_y1 + y;
    clip_x2 = min(clip_x1 + width, copy.clip_x2);
    clip_y2 = min(clip_y1 + height, copy.clip_y2);
}

Graphics::Bitmap::Bitmap(const uint8_t* data, int length){
    loadFromMemory(data, length, false);
}

Graphics::Bitmap::Bitmap(SDL_Surface* surface){
    // SDL_Texture* texture = SDL_CreateTextureFromSurface(global_handler->renderer, surface);
    setData(std::shared_ptr<BitmapData>(new BitmapData(surface, nullptr)));
    
    /*
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    SDL_Point size;
    // FIXME: cache the texture size
    SDL_QueryTexture(this->getData()->texture, NULL, NULL, &size.x, &size.y);
    */

    clip_x1 = 0;
    clip_y1 = 0;
    clip_x2 = surface->w;
    clip_y2 = surface->h;
}

Graphics::Bitmap::Bitmap(SDL_Texture* texture, bool deep_copy){
    /* FIXME: use deep_copy */
    setData(std::shared_ptr<BitmapData>(new BitmapData(nullptr, texture)));

    SDL_Point size;
    // FIXME: cache the texture size
    SDL_QueryTexture(this->getData()->texture, NULL, NULL, &size.x, &size.y);

    clip_x1 = 0;
    clip_y1 = 0;
    clip_x2 = size.x;
    clip_y2 = size.y;
}

Graphics::Bitmap Graphics::Bitmap::createMemoryBitmap(int width, int height){
    return Graphics::Bitmap(width, height);
}

Graphics::Bitmap * Graphics::getScreenBuffer(){
    return Screen;
}

Graphics::Color Graphics::makeColor(int r, int g, int b){
    return makeColor(r, g, b, 255);
}

Graphics::Color Graphics::makeColor(int r, int g, int b, int a){
    INTERNAL_COLOR c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return Graphics::Color(c);
}

void Graphics::Bitmap::enableClip() const {
    SDL_Rect clipRect;
    clipRect.x = clip_x1;
    clipRect.y = clip_y1;
    clipRect.w = clip_x2 - clip_x1;
    clipRect.h = clip_y2 - clip_y1;
    SDL_RenderSetClipRect(global_handler->renderer, &clipRect);
}

void Graphics::Bitmap::disableClip() const {
    SDL_RenderSetClipRect(global_handler->renderer, nullptr);
}

void Graphics::Bitmap::fill(Graphics::Color color) const {
    activate();

    enableClip();
    SDL_SetRenderDrawColor(global_handler->renderer, getRed(color), getGreen(color), getBlue(color), getAlpha(color));
    SDL_Rect* rect = nullptr;
    SDL_RenderFillRect(global_handler->renderer, rect);
    // SDL_RenderClear(global_handler->renderer);
    disableClip();
}

void Graphics::Bitmap::doLoad(Storage::File& file, bool keepInMemory){
    /* FIXME: don't load the entire file into memory, use an rwops instead */
    int length = file.getSize();
    if (length == -1){
        throw BitmapException(__FILE__, __LINE__, std::string("Could not read from file"));
    }
    uint8_t* data = new uint8_t[length];
    try{
        file.readLine((char*) data, length);
        loadFromMemory(data, length, keepInMemory);
        delete[] data;
    } catch (const BitmapException & fail){
        delete[] data;
        throw;
    } catch (...){
        delete[] data;
        throw;
    }

    /*
    SDL_RWops* io = SDL_AllocRW();
    if (io == nullptr){
        DebugLog1 << "Unable to allocate SDL_RWops for file " << endl;
        return;
    }
    SDL_FreeRW(io);
    */
}

int Graphics::Bitmap::getWidth() const {
    return clip_x2 - clip_x1;
}

int Graphics::Bitmap::getHeight() const {
    return clip_y2 - clip_y1;
}

void Graphics::Bitmap::draw(const int x, const int y, Filter * filter, const Bitmap & where) const {
    /* FIXME: handle filter */
    draw(x, y, where);
}

void Graphics::TranslucentBitmap::draw(const int x, const int y, const Graphics::Bitmap & where) const {
    SDL_Texture* texture = getTexture(false);
    if (texture == nullptr){
        return;
    }

    SDL_SetTextureAlphaMod(texture, alpha);
    Graphics::Bitmap::draw(x, y, where);
    SDL_SetTextureAlphaMod(texture, 255);
}

/*
void Graphics::Bitmap::transBlender( int r, int g, int b, int a ){
}
*/

static SDL_Surface* loadFromMemory(const uint8_t* data, int length, bool useMask, const Graphics::Color & maskColor){
    SDL_RWops* ops = SDL_RWFromConstMem(data, length);
    SDL_Surface* surface = IMG_Load_RW(ops, 0);
    SDL_FreeRW(ops);

    if (useMask){
        INTERNAL_COLOR maskColorInternal = maskColor.getInternalColor();
        SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, maskColorInternal.r, maskColorInternal.g, maskColorInternal.b));
    }

    /*
    SDL_Texture* texture = IMG_LoadTexture_RW(global_handler->renderer, ops, 0);
    */

    /*
    SDL_Texture* texture = SDL_CreateTextureFromSurface(global_handler->renderer, surface);
    SDL_FreeSurface(surface);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    */

    // return texture;
    return surface;
}

void Graphics::Bitmap::loadFromMemory(const uint8_t* data, int length, bool keepInMemory){
    /* default mask color is bright pink */
    return loadFromMemory(data, length, makeColor(255, 0, 255), keepInMemory);
}

void Graphics::Bitmap::loadFromMemory(const uint8_t* data, int length, const Color & maskColor, bool keepInMemory){
    SDL_Surface* surface = ::loadFromMemory(data, length, true, maskColor);

    if (surface == nullptr){
        throw BitmapException(__FILE__, __LINE__, std::string("Could not load image from memory"));
    }

    clip_x1 = 0;
    clip_y1 = 0;
    clip_x2 = surface->w;
    clip_y2 = surface->h;

    setData(std::shared_ptr<BitmapData>(new BitmapData(surface, nullptr)));

    /* force texture upload if on the main thread */
    getTexture(keepInMemory);

    /*
    SDL_Point size;
    if (getData() != nullptr){
        // FIXME: cache the texture size
        SDL_QueryTexture(this->getData()->texture, NULL, NULL, &size.x, &size.y);

        clip_x1 = 0;
        clip_y1 = 0;
        clip_x2 = size.x;
        clip_y2 = size.y;
    }
    */
}

SDL_Texture* Graphics::Bitmap::getTexture(bool keepInMemory) const {
    if (getData() == nullptr){
        return nullptr;
    }

    std::shared_ptr<BitmapData> data = getData();
    if (data->texture != nullptr){
        return data->texture;
    }

    /* lazily upload surface to gpu */
    if (Global::isMainThread()){
        if (data->surface != nullptr){
            SDL_Texture* texture = SDL_CreateTextureFromSurface(global_handler->renderer, data->surface);
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

            data->texture = texture;
            if (!keepInMemory){
                SDL_FreeSurface(data->surface);
                data->surface = nullptr;
            }
            return texture;
        }
    }

    return nullptr;
}

void Graphics::Bitmap::internalLoadFile( const char * load_file ){
}

void Graphics::Bitmap::drawHFlip(const int x, const int y, Filter * filter, const Bitmap & where) const {
    /* FIXME: handle filter */
    drawHFlip(x, y, where);
}

Graphics::Color Graphics::Color::updateAlpha(int alpha) const {
    return makeColor(color.r, color.g, color.b, alpha);
}

int Graphics::getRed(const Color & x){
    return x.color.r;
}

int Graphics::getBlue(const Color & x){
    return x.color.b;
}

int Graphics::getGreen(const Color & x){
    return x.color.g;
}

int Graphics::getAlpha(const Color & x){
    return x.color.a;
}

uint32_t getpixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    uint8_t *p = (uint8_t *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            return *p;
            break;

        case 2:
            return *(uint16_t *)p;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            return *(uint32_t *)p;
            break;

        default:
            return 0;       /* shouldn't happen, but avoids warnings */
    }
}

Graphics::Color Graphics::Bitmap::getPixel(const int x, const int y) const {

    if (getData() == nullptr){
        return makeColor(0, 0, 0);
    }

    SDL_Surface* surface = getData()->surface;
    if (surface == nullptr){
        return makeColor(0, 0, 0);
    }

    int true_x = x + clip_x1;
    int true_y = y + clip_y1;

    if (true_x > clip_x2 || true_y > clip_y2){
        return makeColor(0, 0, 0);
    }

    uint8_t red, green, blue;

    SDL_LockSurface(surface);

    uint32_t pixel = getpixel(surface, true_x, true_y);

    SDL_GetRGB(pixel, surface->format, &red, &green, &blue);

    SDL_UnlockSurface(surface);

    return makeColor(red, green, blue);
}

void Graphics::Bitmap::putPixel( int x, int y, Color col ) const {
}

void Graphics::Bitmap::getClipRect( int & x1, int & y1, int & x2, int & y2 ) const {
}

void Graphics::Bitmap::setClipRect( int x1, int y1, int x2, int y2 ) const {
    /*
    clip_x1 = x1;
    clip_y1 = y1;
    clip_x2 = x2;
    clip_y2 = y2;
    */
}

Graphics::Color Graphics::MaskColor(){
    INTERNAL_COLOR color;
    color.r = 0;
    color.g = 0;
    color.b = 0;
    color.a = 0;

    return Color(color);
}

void Graphics::Bitmap::activate() const {
    SDL_Texture* texture = getTexture(false);

    /* when texture is null then we are rendering to the screen */
    if (SDL_GetRenderTarget(global_handler->renderer) != texture){
        int ok = SDL_SetRenderTarget(global_handler->renderer, texture);
        if (ok != 0){
            Global::debug(0, "graphics") << "Unable to set render target: " << SDL_GetError() << endl;
        }
    }
}

void Graphics::initializeExtraStuff(){
}

Graphics::Bitmap::Bitmap(const std::string & load_file){
    Util::ReferenceCount<Storage::File> file = Storage::instance().open(Filesystem::AbsolutePath(load_file));
    if (file != nullptr){
        doLoad(*file.raw(), false);
    }
}

/*
void Graphics::Bitmap::applyTrans(const Color color) const {
}
*/

Graphics::Bitmap Graphics::Bitmap::subBitmap(int x, int y, int width, int height){
    return Graphics::Bitmap(*this, x, y, width, height);
}

void Graphics::Bitmap::save( const std::string & str ) const {
}

Graphics::Color Graphics::Bitmap::blendColor(const Color & input) const {
    return input;
}

void Graphics::Bitmap::startDrawing() const {
}

void Graphics::Bitmap::endDrawing() const {
}

void Graphics::Bitmap::triangle( int x1, int y1, int x2, int y2, int x3, int y3, Color color ) const {
}

void Graphics::Bitmap::ellipse( int x, int y, int rx, int ry, Color color ) const {
}

void Graphics::Bitmap::ellipseFill( int x, int y, int rx, int ry, Color color ) const {
}

void Graphics::Bitmap::light(int x, int y, int width, int height, int start_y, int focus_alpha, int edge_alpha, Color focus_color, Color edge_color) const {
}

void Graphics::Bitmap::rectangle(int x1, int y1, int x2, int y2, Color color) const {
    activate();
    SDL_Rect rect;
    rect.x = x1 + clip_x1;
    rect.y = y1 + clip_y1;
    rect.w = x2 - x1;
    rect.h = y2 - y1;
    enableClip();
    SDL_SetRenderDrawColor(global_handler->renderer, getRed(color), getGreen(color), getBlue(color), getAlpha(color));
    SDL_RenderDrawRect(global_handler->renderer, &rect);
    disableClip();
}

void Graphics::Bitmap::rectangleFill( int x1, int y1, int x2, int y2, Color color ) const {
    activate();
    SDL_Rect rect;
    rect.x = x1 + clip_x1;
    rect.y = y1 + clip_y1;
    rect.w = x2 - x1;
    rect.h = y2 - y1;
    enableClip();
    SDL_SetRenderDrawColor(global_handler->renderer, getRed(color), getGreen(color), getBlue(color), getAlpha(color));
    SDL_RenderFillRect(global_handler->renderer, &rect);
    disableClip();
}

void Graphics::Bitmap::circleFill( int x, int y, int radius, Color color ) const {
    activate();

    enableClip();
    filledCircleRGBA(global_handler->renderer, x + clip_x1, y + clip_y1, radius, getRed(color), getGreen(color), getBlue(color), getAlpha(color));
    disableClip();
    /*
    if (getTexture(false) != nullptr){
        SDL_RenderPresent(global_handler->renderer);
    }
    */
}

void Graphics::Bitmap::circle( int x, int y, int radius, Color color ) const {
}

void Graphics::Bitmap::circle(int x, int y, int radius, int thickness, Color color) const {
}

void Graphics::Bitmap::line( const int x1, const int y1, const int x2, const int y2, const Color color ) const {
    activate();

    enableClip();
    SDL_SetRenderDrawColor(global_handler->renderer, getRed(color), getGreen(color), getBlue(color), getAlpha(color));
    SDL_RenderDrawLine(global_handler->renderer, clip_x1 + x1, clip_y1 + y1, clip_x1 + x2, clip_y1 + y2);
    disableClip();

}

void Graphics::Bitmap::floodfill( const int x, const int y, const Color color ) const {
}

void Graphics::Bitmap::hLine( const int x1, const int y, const int x2, const Color color ) const {
    /*
    activate();

    enableClip();
    SDL_SetRenderDrawColor(global_handler->renderer, getRed(color), getGreen(color), getBlue(color), getAlpha(color));
    SDL_RenderDrawLine(global_handler->renderer, clip_x1 + x, clip_y1 + y, clip_x1 + x2, clip_y1 + y);
    disableClip();
    */
    line(x1, y, x2, y, color);
}

void Graphics::Bitmap::vLine( const int y1, const int x, const int y2, const Color color ) const {
    line(x, y1, x, y2, color);
}

void Graphics::Bitmap::polygon( const int * verts, const int nverts, const Color color ) const {
}

void Graphics::Bitmap::arc(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const {
}

void Graphics::Bitmap::arcFilled(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const {
}

void Graphics::Bitmap::draw(const int x, const int y, const Bitmap & where) const {
    if (getTexture(false) != nullptr){
        SDL_Rect sourceRect;
        sourceRect.x = clip_x1;
        sourceRect.y = clip_y1;
        sourceRect.w = clip_x2 - clip_x1;
        sourceRect.h = clip_y2 - clip_y1;

        where.activate();
        SDL_Rect rect;
        rect.x = x + where.clip_x1;
        rect.y = y + where.clip_y1;

        /*
        SDL_Point size;
        // FIXME: cache the texture size
        SDL_QueryTexture(this->getData()->texture, NULL, NULL, &size.x, &size.y);
        rect.w = size.x;
        rect.h = size.y;
        // DebugLog << "draw size is " << size.x << " " << size.y << endl; 
        */
        rect.w = sourceRect.w;
        rect.h = sourceRect.h;

        where.enableClip();
        SDL_RenderCopy(global_handler->renderer, getTexture(false), &sourceRect, &rect);
        where.disableClip();

        // SDL_RenderCopy(global_handler->renderer, this->getData()->texture, NULL, NULL);
    }
}

void Graphics::Bitmap::drawCharacter( const int x, const int y, const Color color, const int background, const Bitmap & where ) const {
}

void Graphics::Bitmap::drawHFlip(const int x, const int y, const Bitmap & where) const {
    SDL_Texture* texture = getTexture(false);

    if (texture != nullptr){
        where.activate();
        SDL_Rect rect;
        rect.x = x + where.clip_x1;
        rect.y = y + where.clip_y1;
        SDL_Point size;
        // FIXME: cache the texture size
        SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
        rect.w = size.x;
        rect.h = size.y;
        // DebugLog << "draw size is " << size.x << " " << size.y << endl;

        where.enableClip();

        // SDL_RenderCopy(global_handler->renderer, this->getData()->texture, NULL, &rect);

        SDL_RendererFlip flip = SDL_FLIP_HORIZONTAL;
        SDL_RenderCopyEx(global_handler->renderer, texture, NULL, &rect, 0, NULL, flip);

        where.disableClip();

        // SDL_RenderCopy(global_handler->renderer, this->getData()->texture, NULL, NULL);
    }
}

void Graphics::Bitmap::drawVFlip( const int x, const int y, const Bitmap & where ) const {
    SDL_Texture* texture = getTexture(false);

    if (texture != nullptr){
        where.activate();
        SDL_Rect rect;
        rect.x = x + where.clip_x1;
        rect.y = y + where.clip_y1;
        SDL_Point size;
        // FIXME: cache the texture size
        SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
        rect.w = size.x;
        rect.h = size.y;
        // DebugLog << "draw size is " << size.x << " " << size.y << endl;

        where.enableClip();

        // SDL_RenderCopy(global_handler->renderer, this->getData()->texture, NULL, &rect);

        SDL_RendererFlip flip = SDL_FLIP_VERTICAL;
        SDL_RenderCopyEx(global_handler->renderer, texture, NULL, &rect, 0, NULL, flip);

        where.disableClip();

        // SDL_RenderCopy(global_handler->renderer, this->getData()->texture, NULL, NULL);
    }
}

void Graphics::Bitmap::drawVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::Bitmap::drawHVFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::Bitmap::drawHVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::Bitmap::drawStretched(const int x, const int y, const int new_width, const int new_height, const Bitmap & where) const {
    SDL_Texture* texture = getTexture(false);
    if (texture != nullptr){
        where.activate();
        SDL_Rect rect;
        rect.x = x + where.clip_x1;
        rect.y = y + where.clip_y1;
        // SDL_Point size;
        // FIXME: cache the texture size
        // SDL_QueryTexture(this->getData()->texture, NULL, NULL, &size.x, &size.y);
        rect.w = new_width;
        rect.h = new_height;
        // DebugLog << "draw size is " << size.x << " " << size.y << endl;

        where.enableClip();
        SDL_RenderCopy(global_handler->renderer, texture, NULL, &rect);
        where.disableClip();

        // SDL_RenderCopy(global_handler->renderer, this->getData()->texture, NULL, NULL);
    }
}

void Graphics::Bitmap::drawRotate(const int x, const int y, const int angle, const Bitmap & where) const {
    SDL_Texture* texture = getTexture(false);
    if (texture != nullptr){
        where.activate();
        SDL_Rect rect;
        rect.x = x + where.clip_x1;
        rect.y = y + where.clip_y1;
        // SDL_Point size;
        // FIXME: cache the texture size
        // SDL_QueryTexture(this->getData()->texture, NULL, NULL, &size.x, &size.y);
        SDL_Point size;
        // FIXME: cache the texture size
        SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
        rect.w = size.x;
        rect.h = size.y;

        // DebugLog << "draw size is " << size.x << " " << size.y << endl;

        where.enableClip();
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        SDL_RenderCopyEx(global_handler->renderer, texture, NULL, &rect, angle, NULL, flip);
        where.disableClip();

        // SDL_RenderCopy(global_handler->renderer, this->getData()->texture, NULL, NULL);
    }
}

void Graphics::Bitmap::drawPivot( const int centerX, const int centerY, const int x, const int y, const int angle, const Bitmap & where ){
}

void Graphics::Bitmap::drawPivot( const int centerX, const int centerY, const int x, const int y, const int angle, const double scale, const Bitmap & where ){
}

void Graphics::Bitmap::StretchHqx(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight) const {
}

void Graphics::Bitmap::Stretch(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight ) const {
    SDL_Texture* texture = getTexture(false);

    if (texture != nullptr){
        where.activate();
        SDL_Rect destRect;
        destRect.x = destX + where.clip_x1;
        destRect.y = destY + where.clip_y1;
        // SDL_Point size;
        // FIXME: cache the texture size
        // SDL_QueryTexture(this->getData()->texture, NULL, NULL, &size.x, &size.y);
        destRect.w = destWidth;
        destRect.h = destHeight;
        // DebugLog << "draw size is " << size.x << " " << size.y << endl;

        SDL_Rect sourceRect;
        sourceRect.x = this->clip_x1 + sourceX;
        sourceRect.y = this->clip_y1 + sourceY;

        if (sourceWidth + sourceX > this->clip_x2 - this->clip_x1){
            sourceRect.w = this->clip_x2 - this->clip_x1 - sourceX;
        } else {
            sourceRect.w = sourceWidth;
        }
        if (sourceHeight + sourceY > this->clip_y2 - this->clip_y1){
            sourceRect.h = this->clip_y2 - this->clip_y1 - sourceY;
        } else {
            sourceRect.h = sourceHeight;
        }

        where.enableClip();
        SDL_RenderCopy(global_handler->renderer, texture, &sourceRect, &destRect);
        where.disableClip();

        // SDL_RenderCopy(global_handler->renderer, this->getData()->texture, NULL, NULL);
    }
}

void Graphics::Bitmap::StretchBy2( const Bitmap & where ){
}

void Graphics::Bitmap::StretchBy4( const Bitmap & where ){
}

void Graphics::Bitmap::StretchXbr(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight) const {
}

void Graphics::Bitmap::Blit( const int mx, const int my, const int width, const int height, const int wx, const int wy, const Bitmap & where ) const {
    SDL_Texture* texture = getTexture(false);

    if (texture != nullptr){
        where.activate();
        SDL_Rect destRect;
        destRect.x = wx + where.clip_x1;
        destRect.y = wy + where.clip_y1;
        // SDL_Point size;
        // FIXME: cache the texture size
        // SDL_QueryTexture(this->getData()->texture, NULL, NULL, &size.x, &size.y);
        destRect.w = width;
        destRect.h = height;
        // DebugLog << "draw size is " << size.x << " " << size.y << endl;

        SDL_Rect sourceRect;
        sourceRect.x = this->clip_x1 + mx;
        sourceRect.y = this->clip_y1 + my;
        sourceRect.w = width;
        sourceRect.h = height;

        where.enableClip();
        SDL_RenderCopy(global_handler->renderer, texture, &sourceRect, &destRect);
        where.disableClip();

        // SDL_RenderCopy(global_handler->renderer, this->getData()->texture, NULL, NULL);
    }
}

void Graphics::Bitmap::BlitMasked( const int mx, const int my, const int width, const int height, const int wx, const int wy, const Bitmap & where ) const {
}

void Graphics::Bitmap::BlitAreaToScreen(const int upper_left_x, const int upper_left_y) const {
}

void Graphics::Bitmap::BlitToScreen(const int upper_left_x, const int upper_left_y) const {
    SDL_SetRenderTarget(global_handler->renderer, nullptr);
    SDL_RenderPresent(global_handler->renderer);
}

void Graphics::Bitmap::roundRect(int radius, int x1, int y1, int x2, int y2, Color color) const {
}

void Graphics::Bitmap::roundRectFill(int radius, int x1, int y1, int x2, int y2, Graphics::Color color) const {
}

void Graphics::Bitmap::drawShadow(const Bitmap & where, int x, int y, int intensity, Color color, double scale, bool facingRight) const {
}

Graphics::Bitmap Graphics::Bitmap::scaleTo(const int width, const int height) const {
    return Graphics::Bitmap();
}

void Graphics::Bitmap::readLine( std::vector<Color> & vec, int y ){
}

void Graphics::Bitmap::putPixelNormal(int x, int y, Color col) const {
    // set col as the current color
    SDL_SetRenderDrawColor(global_handler->renderer, col.color.r, col.color.g, col.color.b, col.color.a);
    SDL_RenderDrawPoint(global_handler->renderer, x, y);
}

Graphics::Color Graphics::TranslucentBitmap::blendColor(const Color & color) const {
    return color;
}

void Graphics::TranslucentBitmap::fill(Color color) const {
    SDL_SetRenderDrawBlendMode(global_handler->renderer, SDL_BLENDMODE_BLEND);
    Graphics::Bitmap::fill(makeColor(getRed(color), getGreen(color), getBlue(color), alpha));
    SDL_SetRenderDrawBlendMode(global_handler->renderer, SDL_BLENDMODE_NONE);
}

void Graphics::TranslucentBitmap::startDrawing() const {
}

void Graphics::TranslucentBitmap::endDrawing() const {
}

void Graphics::TranslucentBitmap::drawRotate(const int x, const int y, const int angle, const Bitmap & where) const {
    SDL_Texture* texture = getTexture(false);
    if (texture == nullptr){
        return;
    }

    SDL_SetTextureAlphaMod(texture, alpha);
    Graphics::Bitmap::drawRotate(x, y, angle, where);
    SDL_SetTextureAlphaMod(texture, 255);
}

void Graphics::TranslucentBitmap::ellipse( int x, int y, int rx, int ry, Color color ) const {
}

void Graphics::TranslucentBitmap::ellipseFill( int x, int y, int rx, int ry, Color color ) const {
}

void Graphics::TranslucentBitmap::roundRect(int radius, int x1, int y1, int x2, int y2, Color color) const {
}

void Graphics::TranslucentBitmap::roundRectFill(int radius, int x1, int y1, int x2, int y2, Graphics::Color color) const {
}

void Graphics::TranslucentBitmap::circleFill( int x, int y, int radius, Color color ) const {
}

void Graphics::TranslucentBitmap::hLine( const int x1, const int y, const int x2, const Color color ) const {
}

void Graphics::TranslucentBitmap::rectangleFill(int x1, int y1, int x2, int y2, Color color) const {
    SDL_SetRenderDrawBlendMode(global_handler->renderer, SDL_BLENDMODE_BLEND);
    Graphics::Bitmap::rectangleFill(x1, y1, x2, y2, color.updateAlpha(alpha));
    SDL_SetRenderDrawBlendMode(global_handler->renderer, SDL_BLENDMODE_NONE);
}

void Graphics::TranslucentBitmap::rectangle(int x1, int y1, int x2, int y2, Color color) const {
    SDL_SetRenderDrawBlendMode(global_handler->renderer, SDL_BLENDMODE_BLEND);
    Graphics::Bitmap::rectangle(x1, y1, x2, y2, color.updateAlpha(alpha));
    SDL_SetRenderDrawBlendMode(global_handler->renderer, SDL_BLENDMODE_NONE);
}

void Graphics::TranslucentBitmap::line( const int x1, const int y1, const int x2, const int y2, const Color color ) const {
}

void Graphics::TranslucentBitmap::arc(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color) const {
}

void Graphics::TranslucentBitmap::arcFilled(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const {
}

void Graphics::TranslucentBitmap::draw(const int x, const int y, Filter * filter, const Bitmap & where) const {
}

void Graphics::TranslucentBitmap::drawHFlip(const int x, const int y, Filter * filter, const Bitmap & where) const {
}
    
void Graphics::TranslucentBitmap::drawHFlip(const int x, const int y, const Bitmap & where) const {
}

void Graphics::TranslucentBitmap::drawVFlip( const int x, const int y, const Bitmap & where ) const {
    SDL_Texture* texture = getTexture(false);
    if (texture == nullptr){
        return;
    }

    SDL_SetTextureAlphaMod(texture, alpha);
    Graphics::Bitmap::drawVFlip(x, y, where);
    SDL_SetTextureAlphaMod(texture, 255);
}

void Graphics::TranslucentBitmap::drawVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
    /* FIXME: handle filter */
    drawVFlip(x, y, where);
}

void Graphics::TranslucentBitmap::putPixelNormal(int x, int y, Color col) const {
}

void Graphics::TranslucentBitmap::drawHVFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::TranslucentBitmap::drawHVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::LitBitmap::draw(const int x, const int y, const Bitmap & where) const {
    SDL_Texture* texture = getTexture(false);
    if (texture == nullptr){
        return;
    }

    // SDL_SetTextureColorMod(texture, red, green, blue);
    Bitmap::draw(x, y, where);
    // SDL_SetTextureColorMod(texture, 255, 255, 255);
}

void Graphics::LitBitmap::draw(const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::LitBitmap::drawHFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::LitBitmap::drawHFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::LitBitmap::drawVFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::LitBitmap::drawVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::LitBitmap::drawHVFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::LitBitmap::drawHVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

int Graphics::setGfxModeText(){
    return 0;
}

int Graphics::setGraphicsMode(int mode, int width, int height){
    SDL_Window* window = SDL_CreateWindow("paintown", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == nullptr){
        DebugLog << "Could not create a window: " << SDL_GetError() << endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr){
        DebugLog << "Could not create renderer: " << SDL_GetError() << endl;
        return 1;
    }

    SDL_RendererInfo info;
    int ok = SDL_GetRendererInfo(renderer, &info);
    if (ok == 0){
        if (info.flags & SDL_RENDERER_TARGETTEXTURE){
            DebugLog << "Renderer supports rendering to texture" << endl;
        } else {
            DebugLog << "Renderer does not support rendering to texture" << endl;
        }
    } else {
        DebugLog << "Unable to get renderer info: " << SDL_GetError() << endl;
    }

    /*
    double ratio = 640 / (double) 480;
    if (width / ratio > height){
        width = height * ratio;
    } else {
        height = width / ratio;
    }
    */
    // SDL_RenderSetLogicalSize(renderer, width, height);
    /* always render at 640x480 resolution */
    SDL_RenderSetLogicalSize(renderer, 640, 480);

    global_handler = unique_ptr<SDLGlobalHandler>(new SDLGlobalHandler(window, renderer));

    Screen = new Bitmap();
    Screen->clip_x1 = 0;
    Screen->clip_y1 = 0;
    Screen->clip_x2 = 640;
    Screen->clip_y2 = 480;

    return 0;
}

Graphics::StretchedBitmap::StretchedBitmap(int width, int height, const Bitmap & where, Clear clear, QualityFilter filter):
Graphics::Bitmap(width, height),
width(width),
height(height),
scale_x(where.getWidth() / (float) width),
scale_y(where.getHeight() / (float) height),
where(where),
filter(filter),
clearKind(clear){
}

void Graphics::StretchedBitmap::finish(){
    // reset
    // SDL_RenderSetLogicalSize(global_handler->renderer, 0, 0);
    // SDL_RenderSetScale(global_handler->renderer, 1, 1);
    // where.activate();
    // SDL_RenderSetViewport(global_handler->renderer, NULL);
    // SDL_RenderSetScale(global_handler->renderer, 1.0, 1.0);
    drawStretched(where);
}

void Graphics::StretchedBitmap::start(){
    // SDL_RenderSetLogicalSize(global_handler->renderer, width, height);
    // SDL_RenderSetScale(global_handler->renderer, where.getWidth() / (float) width, where.getHeight() / (float) height);
    // SDL_RenderSetScale(global_handler->renderer, scale_x, scale_y);
    /*
    SDL_Rect clipRect;
    clipRect.x = clip_x1;
    clipRect.y = clip_y1;
    clipRect.w = clip_x2 - clip_x1;
    clipRect.h = clip_y2 - clip_y1;
    SDL_RenderSetViewport(global_handler->renderer, &clipRect);
    // SDL_RenderSetScale(global_handler->renderer, scale_x, scale_y);
    SDL_RenderSetScale(global_handler->renderer, 2.0, 2.0);
    */
    // activate();
    // SDL_RenderSetScale(global_handler->renderer, scale_x, scale_y);
}

/*
void Graphics::StretchedBitmap::hLine(const int x1, const int y, const int x2, const Color color ) const {
    where.hLine(x1, y, x2, color);
}
*/

int Graphics::changeGraphicsMode(int mode, int width, int height){
    /*
    double ratio = 640 / (double) 480;
    if (width / ratio > height){
        width = height * ratio;
    } else {
        height = width / ratio;
    }
    */
    // SDL_RenderSetLogicalSize(global_handler->renderer, width, height);

    // SDL_RenderSetLogicalSize(global_handler->renderer, width, height);
    return 0;
}

bool Graphics::Bitmap::getError(){
    /* TODO */
    return false;
}

void Graphics::Bitmap::shutdown(){
    /* close window and renderer safely */
    global_handler = nullptr;
}

Graphics::RestoreState::RestoreState(){
}

Graphics::RestoreState::~RestoreState(){
}

Graphics::TranslatedBitmap::TranslatedBitmap(int x, int y, const Bitmap & where){
}

Graphics::TranslatedBitmap::~TranslatedBitmap(){
}
    
void Graphics::TranslatedBitmap::BlitToScreen() const {
}

void Graphics::Bitmap::lock() const {
}

void Graphics::Bitmap::lock(int x, int y, int width, int height) const {
}

void Graphics::Bitmap::unlock() const {
}

Graphics::Bitmap Graphics::memoryPCX(uint8_t * const data, const int length){
    return Graphics::Bitmap(loadFromMemory(data, length, false, makeColor(0, 0, 0)));
}

Graphics::Bitmap Graphics::memoryPCX(uint8_t * const data, const int length, const Color & maskColor){
    return Graphics::Bitmap(loadFromMemory(data, length, true, maskColor));
}

void Graphics::Bitmap::replaceColor(const Color & original, const Color & replaced){
}

void Graphics::Bitmap::addBlender( int r, int g, int b, int a ){
}

void Graphics::Bitmap::differenceBlender( int r, int g, int b, int a ){
}

void Graphics::Bitmap::alphaBlender(int source, int dest){
}

#endif
