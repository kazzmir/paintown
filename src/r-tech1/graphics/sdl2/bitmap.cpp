#ifdef USE_SDL2

#include "bitmap.h"
#include "../bitmap.h"
#include "../../debug.h"
#include "r-tech1/file-system.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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

Graphics::Bitmap::Bitmap(){
    setData(std::shared_ptr<BitmapData>(new BitmapData(nullptr)));
}

Graphics::Bitmap::Bitmap(int width, int height){
    SDL_Texture* texture = SDL_CreateTexture(global_handler->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);
    setData(std::shared_ptr<BitmapData>(new BitmapData(texture)));

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

Graphics::Bitmap::Bitmap(const char * data, int length){
    /* FIXME: load from PCX data? */
}

Graphics::Bitmap::Bitmap(SDL_Surface* surface){
    SDL_Texture* texture = SDL_CreateTextureFromSurface(global_handler->renderer, surface);
    setData(std::shared_ptr<BitmapData>(new BitmapData(texture)));

    SDL_Point size;
    // FIXME: cache the texture size
    SDL_QueryTexture(this->getData()->texture, NULL, NULL, &size.x, &size.y);

    clip_x1 = 0;
    clip_y1 = 0;
    clip_x2 = size.x;
    clip_y2 = size.y;
}

Graphics::Bitmap::Bitmap(SDL_Texture* texture, bool deep_copy){
    setData(std::shared_ptr<BitmapData>(new BitmapData(texture)));

    SDL_Point size;
    // FIXME: cache the texture size
    SDL_QueryTexture(this->getData()->texture, NULL, NULL, &size.x, &size.y);

    clip_x1 = 0;
    clip_y1 = 0;
    clip_x2 = size.x;
    clip_y2 = size.y;

}

Graphics::Bitmap Graphics::Bitmap::createMemoryBitmap(int width, int height){
    return Graphics::Bitmap();
}

Graphics::Bitmap * Graphics::getScreenBuffer(){
    return Screen;
}

Graphics::Color Graphics::makeColor(int r, int g, int b){
    INTERNAL_COLOR c;
    c.r = r;
    c.g = g;
    c.b = b;
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
    SDL_RenderFillRect(global_handler->renderer, nullptr);
    // SDL_RenderClear(global_handler->renderer);
    disableClip();
}

void Graphics::Bitmap::doLoad(Storage::File& file){
    /* FIXME: don't load the entire file into memory, use an rwops instead */
    int length = file.getSize();
    if (length == -1){
        throw BitmapException(__FILE__, __LINE__, std::string("Could not read from file"));
    }
    char * data = new char[length];
    try{
        file.readLine(data, length);
        loadFromMemory(data, length);
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
}

void Graphics::Bitmap::transBlender( int r, int g, int b, int a ){
}

void Graphics::Bitmap::loadFromMemory(const char * data, int length){
    SDL_RWops* ops = SDL_RWFromConstMem(data, length);

    SDL_Texture* texture = IMG_LoadTexture_RW(global_handler->renderer, ops, 0);
    setData(std::shared_ptr<BitmapData>(new BitmapData(texture)));

    SDL_FreeRW(ops);

    SDL_Point size;

    if (getData() != nullptr){
        // FIXME: cache the texture size
        SDL_QueryTexture(this->getData()->texture, NULL, NULL, &size.x, &size.y);

        clip_x1 = 0;
        clip_y1 = 0;
        clip_x2 = size.x;
        clip_y2 = size.y;
    }
}

void Graphics::Bitmap::internalLoadFile( const char * load_file ){
}

void Graphics::Bitmap::drawHFlip(const int x, const int y, Filter * filter, const Bitmap & where) const {
}

int Graphics::getRed(Color x){
    return x.color.r;
}

int Graphics::getBlue(Color x){
    return x.color.b;
}

int Graphics::getGreen(Color x){
    return x.color.g;
}

int Graphics::getAlpha(Color x){
    return x.color.a;
}

Graphics::Color Graphics::Bitmap::getPixel( const int x, const int y ) const {
    Graphics::Color c;
    return c;
}

void Graphics::Bitmap::putPixel( int x, int y, Color col ) const {
}

void Graphics::Bitmap::getClipRect( int & x1, int & y1, int & x2, int & y2 ) const {
}

void Graphics::Bitmap::setClipRect( int x1, int y1, int x2, int y2 ) const {
}

Graphics::Color Graphics::MaskColor(){
    return makeColor(255, 0, 255);
}

void Graphics::Bitmap::activate() const {
    if (this->getData() != nullptr && SDL_GetRenderTarget(global_handler->renderer) != this->getData()->texture){
        int ok = SDL_SetRenderTarget(global_handler->renderer, this->getData()->texture);
        if (ok != 0){
            Global::debug(0, "graphics") << "Unable to set render target: " << SDL_GetError() << endl;
        }
    }
}

void Graphics::initializeExtraStuff(){
}

Graphics::Bitmap::Bitmap(const std::string & load_file){
}

void Graphics::Bitmap::applyTrans(const Color color) const {
}

Graphics::Bitmap Graphics::Bitmap::subBitmap(int x, int y, int width, int height){
    return Graphics::Bitmap();
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
    rect.x = x1;
    rect.y = y1;
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
    rect.x = x1;
    rect.y = y1;
    rect.w = x2 - x1;
    rect.h = y2 - y1;
    enableClip();
    SDL_SetRenderDrawColor(global_handler->renderer, getRed(color), getGreen(color), getBlue(color), getAlpha(color));
    SDL_RenderFillRect(global_handler->renderer, &rect);
    disableClip();
}

void Graphics::Bitmap::circleFill( int x, int y, int radius, Color color ) const {
    activate();
}

void Graphics::Bitmap::circle( int x, int y, int radius, Color color ) const {
}

void Graphics::Bitmap::circle(int x, int y, int radius, int thickness, Color color) const {
}

void Graphics::Bitmap::line( const int x1, const int y1, const int x2, const int y2, const Color color ) const {
}

void Graphics::Bitmap::floodfill( const int x, const int y, const Color color ) const {
}

void Graphics::Bitmap::hLine( const int x1, const int y, const int x2, const Color color ) const {
}

void Graphics::Bitmap::vLine( const int y1, const int x, const int y2, const Color color ) const {
}

void Graphics::Bitmap::polygon( const int * verts, const int nverts, const Color color ) const {
}

void Graphics::Bitmap::arc(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const {
}

void Graphics::Bitmap::arcFilled(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const {
}

void Graphics::Bitmap::draw(const int x, const int y, const Bitmap & where) const {
    if (this->getData() != nullptr){
        where.activate();
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        SDL_Point size;
        // FIXME: cache the texture size
        SDL_QueryTexture(this->getData()->texture, NULL, NULL, &size.x, &size.y);
        rect.w = size.x;
        rect.h = size.y;
        // DebugLog << "draw size is " << size.x << " " << size.y << endl; 

        enableClip();
        SDL_RenderCopy(global_handler->renderer, this->getData()->texture, NULL, &rect);
        disableClip();

        // SDL_RenderCopy(global_handler->renderer, this->getData()->texture, NULL, NULL);
    }
}

void Graphics::Bitmap::drawCharacter( const int x, const int y, const Color color, const int background, const Bitmap & where ) const {
}

void Graphics::Bitmap::drawHFlip(const int x, const int y, const Bitmap & where) const {
}

void Graphics::Bitmap::drawVFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::Bitmap::drawVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::Bitmap::drawHVFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::Bitmap::drawHVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::Bitmap::drawStretched( const int x, const int y, const int new_width, const int new_height, const Bitmap & who ) const {
}

void Graphics::Bitmap::drawRotate(const int x, const int y, const int angle, const Bitmap & where){
}

void Graphics::Bitmap::drawPivot( const int centerX, const int centerY, const int x, const int y, const int angle, const Bitmap & where ){
}

void Graphics::Bitmap::drawPivot( const int centerX, const int centerY, const int x, const int y, const int angle, const double scale, const Bitmap & where ){
}

void Graphics::Bitmap::StretchHqx(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight) const {
}

void Graphics::Bitmap::Stretch(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight ) const {
}

void Graphics::Bitmap::StretchBy2( const Bitmap & where ){
}

void Graphics::Bitmap::StretchBy4( const Bitmap & where ){
}

void Graphics::Bitmap::StretchXbr(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight) const {
}

void Graphics::Bitmap::Blit( const int mx, const int my, const int width, const int height, const int wx, const int wy, const Bitmap & where ) const {
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

void Graphics::Bitmap::drawShadow(Bitmap & where, int x, int y, int intensity, Color color, double scale, bool facingRight) const {
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

void Graphics::TranslucentBitmap::startDrawing() const {
}

void Graphics::TranslucentBitmap::endDrawing() const {
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
}

void Graphics::TranslucentBitmap::rectangle(int x1, int y1, int x2, int y2, Color color) const {
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
}

void Graphics::TranslucentBitmap::drawVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::TranslucentBitmap::putPixelNormal(int x, int y, Color col) const {
}

void Graphics::TranslucentBitmap::drawHVFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::TranslucentBitmap::drawHVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::LitBitmap::draw(const int x, const int y, const Bitmap & where) const {
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

    global_handler = unique_ptr<SDLGlobalHandler>(new SDLGlobalHandler(window, renderer));

    Screen = new Bitmap();
    Screen->clip_x1 = 0;
    Screen->clip_y1 = 0;
    Screen->clip_x2 = width;
    Screen->clip_y2 = height;

    return 0;
}

Graphics::StretchedBitmap::StretchedBitmap(int width, int height, const Bitmap & where, Clear clear, QualityFilter filter):
where(where),
filter(filter),
clearKind(clear){
}

void Graphics::StretchedBitmap::finish(){
}

void Graphics::StretchedBitmap::start(){
}

int Graphics::changeGraphicsMode(int mode, int width, int height){
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

Graphics::Bitmap Graphics::memoryPCX(unsigned char * const data, const int length, const bool mask){
    return Graphics::Bitmap();
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
