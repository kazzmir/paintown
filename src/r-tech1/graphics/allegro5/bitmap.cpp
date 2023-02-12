#include <sstream>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_memfile.h>
#include <allegro5/allegro_primitives.h>
#include "r-tech1/debug.h"
#include "r-tech1/thread.h"
#include "r-tech1/input/touch.h"
#include "r-tech1/input/input-manager.h"
#include <vector>

namespace Graphics{

ALLEGRO_DISPLAY * the_display = NULL;
static std::vector<Util::ReferenceCount<Shader> > shaders;
// static ALLEGRO_SHADER * shader_default;
static Util::ReferenceCount<Shader> shader_shadow;
static Util::ReferenceCount<Shader> shader_lit_sprite;

enum BlendingType{
    Translucent,
    Add,
    Difference,
    Multiply
};

struct BlendingData{
    BlendingData():
        red(0), green(0), blue(0), alpha(0), type(Translucent){
        }

    int red, green, blue, alpha;
    BlendingType type;
};

static BlendingData globalBlend;

Color makeColorAlpha(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha){
    return Color(al_map_rgba(red, green, blue, alpha));
}

Color MaskColor(){
    static Color mask = makeColorAlpha(0, 0, 0, 0);
    return mask;
}
    
Color getBlendColor(){
    /* sort of a hack */
    if (globalBlend.type == Multiply){
        return makeColorAlpha(255, 255, 255, 255);
    }
    return makeColorAlpha(255, 255, 255, globalBlend.alpha);
}

Color doTransBlend(const Color & color, int alpha){
    unsigned char red, green, blue;
    al_unmap_rgb(color.color, &red, &green, &blue);
    return makeColorAlpha(red, green, blue, alpha);
    /*
    red *= alpha_f;
    green *= alpha_f;
    blue *= alpha_f;
    return al_map_rgb_f(red, green, blue);
    */
}

Color transBlendColor(const Color & color){
    return doTransBlend(color, globalBlend.alpha);
}

int getRealWidth(const Bitmap & what){
    return al_get_bitmap_width(what.getData()->getBitmap());
}

int getRealHeight(const Bitmap & what){
    return al_get_bitmap_height(what.getData()->getBitmap());
}

class Blender{
public:
    Blender(){
    }

    virtual ~Blender(){
        /* default is to draw the source and ignore the destination */
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
    }
};

class MaskedBlender: public Blender {
public:
    MaskedBlender(){
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
    }
};

class LitBlender: public Blender {
public:
    LitBlender(ALLEGRO_COLOR lit){
        // al_set_blend_color(lit);
        // al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE_MINUS_DST_COLOR, ALLEGRO_ZERO);
    }
};

class TransBlender: public Blender {
public:
    TransBlender(){
        switch (globalBlend.type){
            case Translucent: al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA); break;
            case Add: al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE); break;
            case Multiply: al_set_blender(ALLEGRO_ADD, ALLEGRO_DEST_COLOR, ALLEGRO_INVERSE_ALPHA); break;
            case Difference: al_set_blender(ALLEGRO_DEST_MINUS_SRC, ALLEGRO_ONE, ALLEGRO_ONE); break;
        }
    }
};

static const int WINDOWED = 0;
static const int FULLSCREEN = 1;

// static Bitmap * Scaler = NULL;

Bitmap::Bitmap():
mustResize(false),
bit8MaskColor(makeColor(0, 0, 0)),
width(0),
height(0){
    /* TODO */
}

Bitmap::Bitmap( const char * load_file ):
mustResize(false),
bit8MaskColor(makeColor(0, 0, 0)){
    internalLoadFile(load_file);
    width = al_get_bitmap_width(getData()->getBitmap());
    height = al_get_bitmap_height(getData()->getBitmap());
}

Bitmap::Bitmap( const std::string & load_file ):
mustResize(false),
bit8MaskColor(makeColor(0, 0, 0)){
    internalLoadFile(load_file.c_str());
    width = al_get_bitmap_width(getData()->getBitmap());
    height = al_get_bitmap_height(getData()->getBitmap());
}

Bitmap::Bitmap(ALLEGRO_BITMAP * who, bool deep_copy):
mustResize(false),
bit8MaskColor(makeColor(0, 0, 0)){
    if (deep_copy){
        ALLEGRO_BITMAP * clone = al_clone_bitmap(who);
        setData(Util::ReferenceCount<BitmapData>(new BitmapData(clone)));
    } else {
        setData(Util::ReferenceCount<BitmapData>(new BitmapData(who)));
    }
    this->width = al_get_bitmap_width(getData()->getBitmap());
    this->height = al_get_bitmap_height(getData()->getBitmap());
}

Bitmap::Bitmap(int width, int height):
mustResize(false),
bit8MaskColor(makeColor(0, 0, 0)){
    ALLEGRO_BITMAP * bitmap = al_create_bitmap(width, height);
    if (bitmap == NULL){
        std::ostringstream out;
        out << "Could not create bitmap with dimensions " << width << ", " << height;
        throw BitmapException(__FILE__, __LINE__, out.str());
    }
    /*
    if (al_get_bitmap_flags(bitmap) & ALLEGRO_VIDEO_BITMAP){
        ALLEGRO_BITMAP * old = al_get_target_bitmap();
        al_set_target_bitmap(bitmap);
        al_use_shader(shader_default);
        al_set_target_bitmap(old);
    }
    */
    setData(Util::ReferenceCount<BitmapData>(new BitmapData(bitmap)));
    this->width = al_get_bitmap_width(getData()->getBitmap());
    this->height = al_get_bitmap_height(getData()->getBitmap());
}

Bitmap::Bitmap(const Bitmap & copy, bool deep_copy):
mustResize(false),
bit8MaskColor(copy.bit8MaskColor),
width(copy.width),
height(copy.height){
    if (deep_copy){
        ALLEGRO_BITMAP * clone = al_clone_bitmap(copy.getData()->getBitmap());
        setData(Util::ReferenceCount<BitmapData>(new BitmapData(clone)));
    } else {
        setData(copy.getData());
    }
}
        
Bitmap Bitmap::createMemoryBitmap(int width, int height){
    int flags = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    Bitmap out = Bitmap(width, height);
    al_set_new_bitmap_flags(flags);
    return out;
}
        
Bitmap Bitmap::subBitmap(int x, int y, int width, int height){
    ALLEGRO_BITMAP * original = getData()->getBitmap();
    ALLEGRO_BITMAP * sub = al_create_sub_bitmap(original, x, y, width, height);
    return Bitmap(sub, false);
}

void Bitmap::convertToVideo(){
    ALLEGRO_BITMAP * original = getData()->getBitmap();
    ALLEGRO_BITMAP * copy = al_clone_bitmap(original);
    if (copy == NULL){
        throw BitmapException(__FILE__, __LINE__, "Could not create video bitmap");
    }
    al_destroy_bitmap(getData()->getBitmap());
    getData()->setBitmap(copy);

    /*
    if (al_get_bitmap_flags(copy) & ALLEGRO_VIDEO_BITMAP){
        ALLEGRO_BITMAP * old = al_get_target_bitmap();
        al_set_target_bitmap(copy);
        al_use_shader(shader_default);
        al_set_target_bitmap(old);
    }
    */
}

void changeTarget(const Bitmap & from, const Bitmap & who){
    /* pray that if drawing is held then who is already the current target */
    if (!al_is_bitmap_drawing_held()){
        if (al_get_target_bitmap() != who.getData()->getBitmap()){
            al_set_target_bitmap(who.getData()->getBitmap());
        }
        if ((al_get_bitmap_flags(who.getData()->getBitmap()) & ALLEGRO_VIDEO_BITMAP) &&
            (al_get_bitmap_flags(from.getData()->getBitmap()) & ALLEGRO_MEMORY_BITMAP)){

            ((Bitmap&) from).convertToVideo();
            /* How can from == who? If they were the same then the bitmap flags above
             * would not have been different.
             */
            if (&from == &who){
                al_set_target_bitmap(who.getData()->getBitmap());
            }
        }
    }
}

void changeTarget(const Bitmap * from, const Bitmap & who){
    changeTarget(*from, who);
}

void changeTarget(const Bitmap & from, const Bitmap * who){
    changeTarget(from, *who);
}

void changeTarget(const Bitmap * from, const Bitmap * who){
    changeTarget(*from, *who);
}

void dumpColor(const Color & color){
    unsigned char red, green, blue, alpha;
    al_unmap_rgba(color.color, &red, &green, &blue, &alpha);
    Global::debug(0) << "red " << (int) red << " green " << (int) green << " blue " << (int) blue << " alpha " << (int) alpha << std::endl;
}

Color pcxMaskColor(unsigned char * data, const int length){
    if (length >= 769){
        if (data[length - 768 - 1] == 12){
            unsigned char * palette = &data[length - 768];
            unsigned char red = palette[0];
            unsigned char green = palette[1];
            unsigned char blue = palette[2];
            return makeColorAlpha(red, green, blue, 255);
        }
    }
    return makeColorAlpha(255, 255, 255, 255);
}

Bitmap memoryPCX(unsigned char * const data, const int length, const bool mask){
    ALLEGRO_FILE * memory = al_open_memfile((void *) data, length, "r");
    ALLEGRO_BITMAP * pcx = al_load_bitmap_f(memory, ".pcx");
    al_fclose(memory);
    if (pcx == NULL){
        throw BitmapException(__FILE__, __LINE__, "Could not load pcx");
    }
    // dumpColor(al_get_pixel(pcx, 0, 0));
    Bitmap out(pcx);
    out.set8BitMaskColor(pcxMaskColor(data, length));
    return out;
}

static bool isVideoBitmap(ALLEGRO_BITMAP * bitmap){
    return (al_get_bitmap_flags(bitmap) & ALLEGRO_VIDEO_BITMAP) &&
           !(al_get_bitmap_flags(bitmap) & ALLEGRO_MEMORY_BITMAP);
}

void Bitmap::replaceColor(const Color & original, const Color & replaced){
    changeTarget(this, this);

    if (isVideoBitmap(getData()->getBitmap())){
        al_lock_bitmap(getData()->getBitmap(), ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
    }

    int width = getRealWidth(*this);
    int height = getRealHeight(*this);
    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            Color pixel = getPixel(x, y);
            if (pixel == original){
                al_put_pixel(x, y, replaced.color);
            }
        }
    }

    if (isVideoBitmap(getData()->getBitmap())){
        al_unlock_bitmap(getData()->getBitmap());
    }
}

static ALLEGRO_BITMAP * memoryGIF(const char * data, int length){
    ALLEGRO_FILE * memory = al_open_memfile((void *) data, length, "r");
    al_fclose(memory);

/* FIXME: get gif addon for a5 */

#if 0
    RGB * palette = NULL;
    /* algif will close the packfile for us in both error and success cases */
    BITMAP * gif = load_gif_packfile(pack, palette);
    if (!gif){
        al_fclose(memory);
        // pack_fclose(pack);
        ostringstream out;
        out <<"Could not load gif from memory: " << (void*) data << " length " << length;
        throw LoadException(__FILE__, __LINE__, out.str());
    }

    BITMAP * out = create_bitmap(gif->w, gif->h);
    blit(gif, out, 0, 0, 0, 0, gif->w, gif->h);
    destroy_bitmap(gif);
    // pack_fclose(pack);
#endif

    ALLEGRO_BITMAP * out = NULL;

    return out;
}

void Bitmap::internalLoadFile(const char * path){
    this->path = path;
    ALLEGRO_BITMAP * loaded = al_load_bitmap(path);
    if (loaded == NULL){
        std::ostringstream out;
        out << "Could not load file '" << path << "'";
        throw BitmapException(__FILE__, __LINE__, out.str());
    }
    al_convert_mask_to_alpha(loaded, al_map_rgb(255, 0, 255));
    setData(Util::ReferenceCount<BitmapData>(new BitmapData(loaded)));
}

static ALLEGRO_BITMAP * do_load_from_memory(const char * data, int length, const char * type){
    ALLEGRO_FILE * memory = al_open_memfile((void*) data, length, "r");
    ALLEGRO_BITMAP * bitmap = al_load_bitmap_f(memory, type);
    al_fclose(memory);
    al_convert_mask_to_alpha(bitmap, al_map_rgb(255, 0, 255));
    return bitmap;
}

static ALLEGRO_BITMAP * load_bitmap_from_memory(const char * data, int length, ImageFormat type){
    switch (type){
        case FormatBMP: return do_load_from_memory(data, length, ".bmp");
        case FormatPNG: return do_load_from_memory(data, length, ".png");
        case FormatJPG: return do_load_from_memory(data, length, ".jpg");
        case FormatPCX: return do_load_from_memory(data, length, ".pcx");
        case FormatTGA: return do_load_from_memory(data, length, ".tga");
        case FormatTIF: return do_load_from_memory(data, length, ".tif");
        case FormatXPM: return do_load_from_memory(data, length, ".xpm");
        case FormatUnknown: break;
        case FormatGIF : {
            return memoryGIF(data, length);
            break;
        }
    }
    std::ostringstream out;
    out << "Could not load the bitmap because its format was not known";
    throw BitmapException(__FILE__, __LINE__, out.str());
}

Bitmap::Bitmap(const char * data, int length):
mustResize(false),
bit8MaskColor(makeColor(0, 0, 0)){
    loadFromMemory(data, length);
}

void Bitmap::loadFromMemory(const char * data, int length){
    setData(Util::ReferenceCount<BitmapData>(new BitmapData(load_bitmap_from_memory(data, length, identifyImage((const unsigned char *) data, length)))));
    if (getData()->getBitmap() == NULL){
        std::ostringstream out;
        out << "Could not create bitmap from memory";
        throw BitmapException(__FILE__, __LINE__, out.str());
    }
    width = al_get_bitmap_width(getData()->getBitmap());
    height = al_get_bitmap_height(getData()->getBitmap());
}

Bitmap::Bitmap( const Bitmap & copy, int x, int y, int width, int height ):
mustResize(false),
bit8MaskColor(copy.bit8MaskColor),
width(width),
height(height){
    path = copy.getPath();
    ALLEGRO_BITMAP * his = copy.getData()->getBitmap();
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (width < 1 || height < 1){
        std::ostringstream out;
        out << "Attempting to create a sub-bitmap.";
        if (width < 1){
            out << " Width was less than 1: " << width;
        }
        if (height < 1){
            out << " Height was less than 1: " << height;
        }
        throw BitmapException(__FILE__, __LINE__, out.str());
    }
    /*
    if (width + x > al_get_bitmap_width(his)){
        width = al_get_bitmap_width(his) - x;
    }
    if (height + y > al_get_bitmap_height(his)){
        height = al_get_bitmap_height(his) - y;
    }
    */
    
    ALLEGRO_BITMAP * old_target = al_get_target_bitmap();
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    if (al_get_target_bitmap() != copy.getData()->getBitmap()){
        al_set_target_bitmap(copy.getData()->getBitmap());
    }
    if (al_get_current_transform() != NULL){
        if (old_target != copy.getData()->getBitmap()){
            al_set_target_bitmap(copy.getData()->getBitmap());
        }
        al_copy_transform(&transform, al_get_current_transform());
    }

    float x_scaled = x;
    float y_scaled = y;
    float width_scaled = width;
    float height_scaled = height;

    al_transform_coordinates(&transform, &x_scaled, &y_scaled);
    al_transform_coordinates(&transform, &width_scaled, &height_scaled);

    // ALLEGRO_BITMAP * sub = al_create_sub_bitmap(his, x, y, width, height);
    ALLEGRO_BITMAP * sub = al_create_sub_bitmap(his, (int) x_scaled, (int) y_scaled, (int) width_scaled, (int) height_scaled);
    // ALLEGRO_BITMAP * sub = al_create_sub_bitmap(his, (int) x_scaled, (int) y_scaled, width, height);
    setData(Util::ReferenceCount<BitmapData>(new BitmapData(sub)));

    al_set_target_bitmap(sub);
    al_use_transform(&transform);
    al_set_target_bitmap(old_target);
}

static bool isBackBuffer(ALLEGRO_BITMAP * bitmap){
    return bitmap == al_get_backbuffer(the_display);
}

int Bitmap::getWidth() const {
    /* Always return the true dimensions of the backbuffer */
    if (getData() != NULL && isBackBuffer(getData()->getBitmap())){
        return al_get_bitmap_width(getData()->getBitmap());
    }
    return width;
    /*
    if (getData()->getBitmap() != NULL){
        return al_get_bitmap_width(getData()->getBitmap());
    }

    return 0;
    */
}

int getRed(Color color){
    unsigned char red, green, blue;
    al_unmap_rgb(color.color, &red, &green, &blue);
    return red;
}

int getAlpha(Color color){
    unsigned char red, green, blue, alpha;
    al_unmap_rgba(color.color, &red, &green, &blue, &alpha);
    return alpha;
}

int getGreen(Color color){
    unsigned char red, green, blue;
    al_unmap_rgb(color.color, &red, &green, &blue);
    return green;
}

int getBlue(Color color){
    unsigned char red, green, blue;
    al_unmap_rgb(color.color, &red, &green, &blue);
    return blue;
}

Color makeColor(int red, int blue, int green){
    return Color(al_map_rgb(red, blue, green));
}

int Bitmap::getHeight() const {
    if (getData() != NULL && isBackBuffer(getData()->getBitmap())){
        return al_get_bitmap_height(getData()->getBitmap());
    }
    return height;
    /*
    if (getData()->getBitmap() != NULL){
        return al_get_bitmap_height(getData()->getBitmap());
    }

    return 0;
    */
}

void initializeExtraStuff(){
    // al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_RGB_565);
}

std::string defaultVertexShader(){
    return std::string(al_get_default_shader_source(ALLEGRO_SHADER_GLSL, ALLEGRO_VERTEX_SHADER));
}

std::string defaultPixelShader(){
    return std::string(al_get_default_shader_source(ALLEGRO_SHADER_GLSL, ALLEGRO_PIXEL_SHADER));
}
    
void setShaderSampler(ALLEGRO_SHADER * shader, const std::string & name, const Bitmap & texture, int unit){
    al_set_shader_sampler(name.c_str(), texture.getData()->getBitmap(), unit); 
}

void setShaderBool(ALLEGRO_SHADER * shader, const std::string & name, bool value){
    setShaderInt(shader, name, value);
}

void setShaderInt(ALLEGRO_SHADER * shader, const std::string & name, int value){
    al_set_shader_int(name.c_str(), value); 
}

void setShaderFloat(ALLEGRO_SHADER * shader, const std::string & name, float value){
    al_set_shader_float(name.c_str(), value); 
}

void setShaderVec4(ALLEGRO_SHADER * shader, const std::string & name, float v1, float v2, float v3, float v4){
    float vector[4];
    vector[0] = v1;
    vector[1] = v2;
    vector[2] = v3;
    vector[3] = v4;

    al_set_shader_float_vector(name.c_str(), 4, &vector[0], 1);
}

static std::string shader_version(const std::string & version, const std::string & data){
    std::ostringstream out;
    out << "#version " << version << "\n";
    out << data;
    return out.str();
}

Util::ReferenceCount<Shader> create_shader(const std::string & version, const std::string & vertex, const std::string & pixel){
    ALLEGRO_SHADER * shader = al_create_shader(ALLEGRO_SHADER_GLSL);
    if (shader == NULL){
        return Util::ReferenceCount<Shader>(NULL);
    }
    if (!al_attach_shader_source(shader, ALLEGRO_VERTEX_SHADER, shader_version(version, vertex).c_str())){
        Global::debug(0) << "attach vertex shader source failed: " << al_get_shader_log(shader) << std::endl << vertex << std::endl;
        return Util::ReferenceCount<Shader>(NULL);
    }
    if (!al_attach_shader_source(shader, ALLEGRO_PIXEL_SHADER, shader_version(version, pixel).c_str())){
        Global::debug(0) << "attach pixel shader source failed: " << al_get_shader_log(shader) << std::endl << pixel << std::endl;
        Global::debug(0) << "Pixel source: \n" << shader_version(version, pixel) << std::endl;
        return Util::ReferenceCount<Shader>(NULL);
    }
    if (!al_build_shader(shader)){
        Global::debug(0) << "shader al_link_shader failed: " << al_get_shader_log(shader) << " pixel: " << pixel << " vertex: " << vertex << std::endl;
        return Util::ReferenceCount<Shader>(NULL);
    }
    return Util::ReferenceCount<Shader>(new Shader(shader));
}

int changeGraphicsMode(int mode, int width, int height){
    switch (mode){
        case FULLSCREEN: {
            al_set_display_flag(the_display, ALLEGRO_FULLSCREEN_WINDOW, true);
            break;
        }
        case WINDOWED: {
            al_set_display_flag(the_display, ALLEGRO_FULLSCREEN_WINDOW, false);
            break;
        }
    }
    return !al_resize_display(the_display, width, height);
}

static int createShaders(){
    try{
        shader_shadow = create_shader("100", defaultVertexShader(), Storage::readFile(Storage::instance().find(Filesystem::RelativePath("shaders/shadow.fragment.glsl"))));
        if (shader_shadow == NULL){
            Global::debug(0) << "Could not create shader for shaders/shadow.fragment.glsl" << std::endl;
            return 1;
        }
        shaders.push_back(shader_shadow);
        Global::debug(1) << "Created shadow shader" << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Could not load shadow shader: " << fail.getTrace() << std::endl;
        return 1;
    }

    try{
        shader_lit_sprite = create_shader("100", defaultVertexShader(), Storage::readFile(Storage::instance().find(Filesystem::RelativePath("shaders/lit-sprite.fragment.glsl"))));
        if (shader_lit_sprite == NULL){
            return 1;
        }
        shaders.push_back(shader_lit_sprite);
        Global::debug(1) << "Created lit sprite shader" << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Could not load lit sprite shader: " << fail.getTrace() << std::endl;
        return 1;
    }

    return 0;
}

int setGraphicsMode(int mode, int width, int height){
    initializeExtraStuff();

    /* FIXME: the configuration should pass in fullscreen mode here */
#ifdef IPHONE
    mode = FULLSCREEN;
#endif
    switch (mode){
        case FULLSCREEN: {
#ifdef IPHONE
            al_set_new_display_option(ALLEGRO_SUPPORTED_ORIENTATIONS, ALLEGRO_DISPLAY_ORIENTATION_LANDSCAPE, ALLEGRO_SUGGEST);
            al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW | ALLEGRO_PROGRAMMABLE_PIPELINE);
#else

#ifdef ANDROID
            al_set_new_display_flags(ALLEGRO_FULLSCREEN | ALLEGRO_PROGRAMMABLE_PIPELINE);
#else
            al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW | ALLEGRO_RESIZABLE |
                                     ALLEGRO_OPENGL | ALLEGRO_PROGRAMMABLE_PIPELINE);
#endif
#endif
            break;
        }
        case WINDOWED: {
            al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE |
                                     ALLEGRO_PROGRAMMABLE_PIPELINE |
                                     ALLEGRO_OPENGL);
            break;
        }
        default: break;
    }
    the_display = al_create_display(width, height);
    if (the_display == NULL){
        std::ostringstream out;
        out << "Could not create display with dimensions " << width << ", " << height;
        throw BitmapException(__FILE__, __LINE__, out.str());
    }

    // Global::debug(0) << "Set width " << al_get_display_width(the_display) << " height " << al_get_display_height(the_display) << std::endl;
    // Global::debug(0) << "Backbuffer width " << al_get_bitmap_width(al_get_backbuffer(the_display)) << " height " << al_get_bitmap_height(al_get_backbuffer(the_display)) << std::endl;

    try{
        /* TODO: maybe find a more general way to get the icon */
        ALLEGRO_BITMAP * icon = al_load_bitmap(Storage::instance().find(Filesystem::RelativePath("menu/icon.bmp")).path().c_str());
        if (icon != NULL){
            al_set_display_icon(the_display, icon);
        }
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Could not set window icon: " << fail.getTrace() << std::endl;
    }
    Screen = new Bitmap(al_get_backbuffer(the_display));
    /* dont destroy the backbuffer */
    Screen->getData()->setDestroy(false);

    /*
    ALLEGRO_TRANSFORM transformation;
    al_identity_transform(&transformation);
    al_scale_transform(&transformation, (double) Screen->getWidth() / (double) width, (double) Screen->getHeight() / (double) height);
    al_set_target_bitmap(Screen->getData()->getBitmap());
    al_use_transform(&transformation);
    */

    // Scaler = new Bitmap(width, height);
    /* default drawing mode */
    // al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    // shader_default = create_shader(defaultVertexShader(), defaultPixelShader());
    // al_use_shader(shader_default);

    /* Default shader */
    /*
    shader_default = al_create_shader(ALLEGRO_SHADER_GLSL);
    al_attach_shader_source(shader_default, ALLEGRO_VERTEX_SHADER, al_get_default_glsl_vertex_shader());
    al_attach_shader_source(shader_default, ALLEGRO_PIXEL_SHADER, al_get_default_glsl_pixel_shader());
    if (!al_link_shader(shader_default)){
        Global::debug(0) << "default shader al_link_shader failed: " << al_get_shader_log(shader_default) << std::endl;
        return 1;
    }
    */
    // al_set_shader(the_display, shader_default);
    // shaders.push_back(shader_default);
    // Global::debug(1) << "Created default shader" << std::endl;

    if (createShaders()){
        return 1;
    }

    return 0;
}

void Bitmap::lock() const {
    al_lock_bitmap(getData()->getBitmap(), ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
}
        
void Bitmap::lock(int x, int y, int width, int height) const {
    al_lock_bitmap_region(getData()->getBitmap(), x, y, width, height, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
}

void Bitmap::unlock() const {
    al_unlock_bitmap(getData()->getBitmap());
}

Color Bitmap::getPixel(const int x, const int y) const {
    // changeTarget(this, this);
    return Color(al_get_pixel(getData()->getBitmap(), x, y));
}

void Bitmap::putPixel(int x, int y, Color pixel) const {
    changeTarget(this, this);
    // al_put_pixel(x, y, pixel);
    al_draw_pixel(x, y, pixel.color);
}

void Bitmap::putPixelNormal(int x, int y, Color col) const {
    putPixel(x, y, col);
    /*
    changeTarget(this, this);
    al_put_pixel(x, y, col.color);
    */
}

void Bitmap::fill(Color color) const {
    changeTarget(this, this);
    al_clear_to_color(color.color);
}

void Bitmap::startDrawing() const {
    /* we are about to draw on this bitmap so make sure we are the target */
    changeTarget(this, this);
    al_hold_bitmap_drawing(true);
}

void Bitmap::endDrawing() const {
    al_hold_bitmap_drawing(false);
}

void TranslucentBitmap::startDrawing() const {
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
}

void TranslucentBitmap::endDrawing() const {
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
}

Color Bitmap::blendColor(const Color & input) const {
    return input;
}

Color TranslucentBitmap::blendColor(const Color & color) const {
    unsigned char red, green, blue;
    unsigned char alpha = globalBlend.alpha;
    al_unmap_rgb(color.color, &red, &green, &blue);
    return makeColorAlpha(red, green, blue, alpha);
}

void Bitmap::StretchHqx(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight) const {
    /* TODO */
}

void Bitmap::StretchXbr(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight) const {
    /* TODO */
}

void Bitmap::Stretch(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight) const {
    changeTarget(this, where);
    al_draw_scaled_bitmap(getData()->getBitmap(),
                          sourceX, sourceY, sourceWidth, sourceHeight,
                          destX, destY, destWidth, destHeight,
                          0);
}

void Bitmap::StretchBy2(const Bitmap & where){
    Stretch(where, 0, 0, getWidth(), getHeight(), 0, 0, getWidth() * 2, getHeight() * 2);
}

void Bitmap::StretchBy4(const Bitmap & where){
    Stretch(where, 0, 0, getWidth(), getHeight(), 0, 0, getWidth() * 4, getHeight() * 4);
}

void Bitmap::drawRotate(const int x, const int y, const int angle, const Bitmap & where ){
    changeTarget(this, where);
    MaskedBlender blender;
    al_draw_rotated_bitmap(getData()->getBitmap(), getWidth() / 2, getHeight() / 2, x, y, Util::radians(angle), ALLEGRO_FLIP_HORIZONTAL);
}

void Bitmap::drawPivot( const int centerX, const int centerY, const int x, const int y, const int angle, const Bitmap & where ){
    changeTarget(this, where);
    MaskedBlender blender;
    al_draw_rotated_bitmap(getData()->getBitmap(), getWidth() / 2, getHeight() / 2, x, y, Util::radians(-angle), 0);
}

void Bitmap::drawPivot( const int centerX, const int centerY, const int x, const int y, const int angle, const double scale, const Bitmap & where ){
    /* TODO */
}

void Bitmap::drawStretched( const int x, const int y, const int new_width, const int new_height, const Bitmap & who ) const {
    /* FIXME */
    changeTarget(this, who);
    MaskedBlender blender;
    al_draw_scaled_bitmap(getData()->getBitmap(), 0, 0, al_get_bitmap_width(getData()->getBitmap()), al_get_bitmap_height(getData()->getBitmap()), x, y, new_width, new_height, 0);
#if 0
    ALLEGRO_TRANSFORM save;
    al_copy_transform(&save, al_get_current_transform());
    ALLEGRO_TRANSFORM stretch;
    al_identity_transform(&stretch);
    // al_translate_transform(&stretch, x / ((double) new_width / getWidth()), y / ((double) new_height / getHeight()));
    al_scale_transform(&stretch, (double) new_width / getWidth(), (double) new_height / getHeight());
    al_translate_transform(&stretch, x, y);
    // al_translate_transform(&stretch, -x / ((double) new_width / getWidth()), -y / ((double) (new_height / getHeight())));
    al_use_transform(&stretch);
    /* any source pixels with an alpha value of 0 will be masked */
    // al_draw_bitmap(getData().getBitmap(), x, y, 0);
    al_draw_bitmap(getData().getBitmap(), 0, 0, 0);
    al_use_transform(&save);
#endif
}

Bitmap Bitmap::scaleTo(const int width, const int height) const {
    if (width == getRealWidth(*this) && height == getRealHeight(*this)){
        return *this;
    }
    Bitmap scaled(width, height);
    changeTarget(*this, scaled);
    al_draw_scaled_bitmap(getData()->getBitmap(), 0, 0, getRealWidth(*this), getRealHeight(*this),
                          0, 0, width, height, 0);
    return scaled;
}

void Bitmap::Blit(const int mx, const int my, const int width, const int height, const int wx, const int wy, const Bitmap & where) const {
    // double start = al_get_time();
    // changeTarget(this, where);
    // al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
    /*
    if (&where != Screen){
        al_draw_bitmap(getData().getBitmap(), wx, wy, 0);
    }
    */

    changeTarget(this, where);
    Bitmap part(*this, mx, my, width, height);
    // al_use_shader(shader_default);
    // al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
    al_draw_bitmap(part.getData()->getBitmap(), wx, wy, 0);
    /*
    double end = al_get_time();
    Global::debug(0) << "Draw in " << (end - start) << " seconds" << std::endl;
    */
}

void Bitmap::drawHFlip(const int x, const int y, const Bitmap & where) const {
    draw(x, y, NULL, where, ALLEGRO_FLIP_HORIZONTAL);
}

void Bitmap::drawHFlip(const int x, const int y, Filter * filter, const Bitmap & where) const {
    draw(x, y, filter, where, ALLEGRO_FLIP_HORIZONTAL);
}

void Bitmap::drawVFlip( const int x, const int y, const Bitmap & where ) const {
    draw(x, y, NULL, where, ALLEGRO_FLIP_VERTICAL);
}

void Bitmap::drawVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
    draw(x, y, filter, where, ALLEGRO_FLIP_VERTICAL);
}

void Bitmap::drawHVFlip( const int x, const int y, const Bitmap & where ) const {
    draw(x, y, NULL, where, ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL);
}

void Bitmap::drawHVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
    draw(x, y, filter, where, ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL);
}

void Bitmap::BlitMasked(const int mx, const int my, const int width, const int height, const int wx, const int wy, const Bitmap & where) const {
    /* TODO */
}

void Bitmap::BlitToScreen(const int upper_left_x, const int upper_left_y) const {
#if 0
    if (getWidth() != Screen->getWidth() || getHeight() != Screen->getHeight()){
        /*
        this->Blit( upper_left_x, upper_left_y, *Buffer );
        Buffer->Stretch(*Scaler);
        Scaler->Blit(0, 0, 0, 0, *Screen);
        */

        this->Stretch(*Scaler, 0, 0, getWidth(), getHeight(), upper_left_x, upper_left_y, Scaler->getWidth(), Scaler->getHeight());
        Scaler->Blit(0, 0, 0, 0, *Screen);
    } else {
        this->Blit(upper_left_x, upper_left_y, *Screen);
    }
#endif
    /*
    if (&where == Screen){
        al_flip_display();
    }
    */
    changeTarget(this, Screen);
    if (getData()->getBitmap() != Screen->getData()->getBitmap()){
        Blit(*Screen);
    }
    al_flip_display();
}

void Bitmap::BlitAreaToScreen(const int upper_left_x, const int upper_left_y) const {
    changeTarget(this, Screen);
    /*
    if (getData()->getBitmap() != Screen->getData()->getBitmap()){
        Blit(upper_left_y, upper_left_y, *Screen);
    }
    */
    al_flip_display();
}

void Bitmap::draw(const int x, const int y, Filter * filter, const Bitmap & where, int flags) const {
    changeTarget(this, where);
    MaskedBlender blender;
    Util::ReferenceCount<Shader> shader;
    if (filter != NULL){
        shader = filter->getShader();
    }

    ALLEGRO_SHADER * a5shader = NULL;
    if (shader != NULL){
        a5shader = shader->getShader();
    }

    if (a5shader != NULL){
        al_use_shader(a5shader);
        filter->setupShader(shader);
    }

    /* any source pixels with an alpha value of 0 will be masked */
    al_draw_bitmap(getData()->getBitmap(), x, y, flags);

    if (a5shader != NULL){
        al_use_shader(NULL);
    }
}

void Bitmap::draw(const int x, const int y, const Bitmap & where) const {
    draw(x, y, NULL, where, 0);

    /*
    // TransBlender blender;
    changeTarget(this, where);
    MaskedBlender blender;
    / * any source pixels with an alpha value of 0 will be masked * /
    al_draw_bitmap(getData()->getBitmap(), x, y, 0);
    */
}

void Bitmap::draw(const int x, const int y, Filter * filter, const Bitmap & where) const {
    draw(x, y, filter, where, 0);
}

void Bitmap::drawShadow(Bitmap & where, int x, int y, int intensity, Color color, double scale, bool facingRight) const {
    changeTarget(this, where);
    MaskedBlender blender;
    int newHeight = fabs(scale) * getHeight();
    int flags = 0;
    if (!facingRight){
        flags |= ALLEGRO_FLIP_HORIZONTAL;
    }
    float shadowColor[4];
    al_unmap_rgb_f(color.color, &shadowColor[0], &shadowColor[1], &shadowColor[2]);
    shadowColor[3] = (float) intensity / 255.0;
    al_use_shader(shader_shadow->getShader());
    if (!al_set_shader_float_vector("shadow", 4, shadowColor, 1)){
        /* Well.. thats not good. Did the shader source get messed up? */
    }
    al_draw_scaled_bitmap(getData()->getBitmap(), 0, 0, getWidth(), getHeight(), x, y - newHeight, getWidth(), newHeight, flags);
    al_use_shader(NULL);
}

void Bitmap::hLine(const int x1, const int y, const int x2, const Color color) const {
    line(x1, y, x2, y, color);
}

void Bitmap::vLine(const int y1, const int x, const int y2, const Color color) const {
    line(x, y1, x, y2, color);
}

void Bitmap::arc(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const {
    changeTarget(this, this);
    al_draw_arc(x, y, radius, ang1 - Util::pi/2, ang2 - ang1, color.color, 1);
}

void TranslucentBitmap::arc(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const {
    TransBlender blender;
    Bitmap::arc(x, y, ang1, ang2, radius, transBlendColor(color));
    /*
    changeTarget(this);
    al_draw_arc(x, y, radius, ang1 + S_PI/2, ang2 - ang1, doTransBlend(color, globalBlend.alpha), 0);
    */
}

/* from http://www.allegro.cc/forums/thread/605684/892721#target */
#if 0
void al_draw_filled_pieslice(float cx, float cy, float r, float start_theta,
                                float delta_theta, ALLEGRO_COLOR color){
    ALLEGRO_VERTEX vertex_cache[ALLEGRO_VERTEX_CACHE_SIZE];
    int num_segments, ii;

    num_segments = fabs(delta_theta / (2 * ALLEGRO_PI) * ALLEGRO_PRIM_QUALITY * sqrtf(r));

    if (num_segments < 2)
        return;

    if (num_segments >= ALLEGRO_VERTEX_CACHE_SIZE) {
        num_segments = ALLEGRO_VERTEX_CACHE_SIZE - 1;
    }

    al_calculate_arc(&(vertex_cache[1].x), sizeof(ALLEGRO_VERTEX), cx, cy, r, r, start_theta, delta_theta, 0, num_segments);
    vertex_cache[0].x = cx; vertex_cache[0].y = cy;

    for (ii = 0; ii < num_segments + 1; ii++) {
        vertex_cache[ii].color = color;
        vertex_cache[ii].z = 0;
    }

    al_draw_prim(vertex_cache, NULL, NULL, 0, num_segments + 1, ALLEGRO_PRIM_TRIANGLE_FAN);
    // al_draw_prim(vertex_cache, NULL, NULL, 0, 3, ALLEGRO_PRIM_TRIANGLE_FAN);
}
#endif

static float min(float a, float b){
    return a < b ? a : b;
}

void Bitmap::roundRect(int radius, int x1, int y1, int x2, int y2, Color color) const {
    changeTarget(this, this);
    double mx = x2 - x1;
    double my = y2 - y1;
    double radius_use = min(min(radius, mx / 2), my / 2);
    al_draw_rounded_rectangle(x1, y1, x2, y2, radius_use, radius_use, color.color, 1);
}

void TranslucentBitmap::roundRect(int radius, int x1, int y1, int x2, int y2, Color color) const {
    TransBlender blender;
    Bitmap::roundRect(radius, x1, y1, x2, y2, transBlendColor(color));
}

void Bitmap::roundRectFill(int radius, int x1, int y1, int x2, int y2, Graphics::Color color) const {
    changeTarget(this, this);
    double mx = x2 - x1;
    double my = y2 - y1;
    double radius_use = min(min(radius, mx / 2), my / 2);
    al_draw_filled_rounded_rectangle(x1, y1, x2, y2, radius_use, radius_use, color.color);
}
    
void TranslucentBitmap::roundRectFill(int radius, int x1, int y1, int x2, int y2, Graphics::Color color) const {
    TransBlender blender;
    Bitmap::roundRectFill(radius, x1, y1, x2, y2, transBlendColor(color));
}

void Bitmap::arcFilled(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const {
    changeTarget(this, this);
    al_draw_filled_pieslice(x, y, radius, ang1 - Util::pi/2, ang2 - ang1, color.color);
}

void TranslucentBitmap::arcFilled(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const {
    changeTarget(this, this);
    TransBlender blender;
    Bitmap::arcFilled(x, y, ang1, ang2, radius, transBlendColor(color));
}

void Bitmap::floodfill( const int x, const int y, const Color color ) const {
    /* TODO */
}

void Bitmap::line(const int x1, const int y1, const int x2, const int y2, const Color color) const {
    changeTarget(this, this);
    al_draw_line(x1, y1, x2, y2, color.color, 1);
}

void TranslucentBitmap::line(const int x1, const int y1, const int x2, const int y2, const Color color) const {
    TransBlender blender;
    Bitmap::line(x1, y1, x2, y2, transBlendColor(color));
}

void Bitmap::circleFill(int x, int y, int radius, Color color) const {
    changeTarget(this, this);
    al_draw_filled_circle(x, y, radius, color.color);
}

void Bitmap::circle(int x, int y, int radius, Color color) const {
    changeTarget(this, this);
    al_draw_circle(x, y, radius, color.color, 1);
}

void Bitmap::circle(int x, int y, int radius, int thickness, Color color) const {
    changeTarget(this, this);
    al_draw_circle(x, y, radius, color.color, thickness);
}

void Bitmap::rectangle(int x1, int y1, int x2, int y2, Color color ) const {
    changeTarget(this, this);
    // al_draw_rectangle(x1, y1, x2, y2, color.color, 0);
    al_draw_rectangle(x1 + 0.5, y1 + 0.5, x2 - 0.5, y2 - 0.5, color.color, 1);
}

void Bitmap::rectangleFill( int x1, int y1, int x2, int y2, Color color ) const {
    changeTarget(this, this);
    // al_draw_filled_rectangle(x1 - 0.5, y1 - 0.5, x2 + 0.5, y2 + 0.5, color.color);
    al_draw_filled_rectangle(x1, y1, x2 + 1, y2 + 1, color.color);
}

void Bitmap::triangle( int x1, int y1, int x2, int y2, int x3, int y3, Color color ) const {
    changeTarget(this, this);
    al_draw_filled_triangle(x1, y1, x2, y2, x3, y3, color.color);
}

void Bitmap::polygon( const int * verts, const int nverts, const Color color ) const {
    /* TODO */
}

void Bitmap::ellipse( int x, int y, int rx, int ry, Color color ) const {
    changeTarget(this, this);
    al_draw_ellipse(x, y, rx, ry, color.color, 0);
}

void Bitmap::ellipseFill( int x, int y, int rx, int ry, Color color ) const {
    changeTarget(this, this);
    al_draw_filled_ellipse(x, y, rx, ry, color.color);
}

void Bitmap::applyTrans(const Color color) const {
    TransBlender blender;
    changeTarget(this, this);
    al_draw_filled_rectangle(0, 0, getWidth(), getHeight(), transBlendColor(color).color);
}

void Bitmap::light(int x, int y, int width, int height, int start_y, int focus_alpha, int edge_alpha, Color focus_color, Color edge_color) const {
    /* TODO */
}

void Bitmap::drawCharacter( const int x, const int y, const Color color, const int background, const Bitmap & where ) const {
    /* TODO */
}

void Bitmap::save( const std::string & str ) const {
    /* TODO */
}

void Bitmap::readLine(std::vector<Color> & line, int y){
    /* TODO */
}

void TranslucentBitmap::draw(const int x, const int y, Filter * filter, const Bitmap & where, int flags) const {
    changeTarget(this, where);
    TransBlender blender;
    Util::ReferenceCount<Shader> shader;
    if (filter != NULL){
        shader = filter->getShader();
    }

    ALLEGRO_SHADER * a5shader = NULL;
    if (shader != NULL){
        a5shader = shader->getShader();
    }

    if (a5shader != NULL){
        al_use_shader(a5shader);
        filter->setupShader(shader);
    }

    al_draw_tinted_bitmap(getData()->getBitmap(), getBlendColor().color, x, y, flags);

    if (a5shader != NULL){
        al_use_shader(NULL);
    }
}

void TranslucentBitmap::draw(const int x, const int y, const Bitmap & where) const {
    draw(x, y, NULL, where, 0);
}

void LitBitmap::draw(const int x, const int y, const Bitmap & where) const {
    draw(x, y, NULL, where, 0);
}

void LitBitmap::draw(const int x, const int y, Filter * filter, const Bitmap & where, int flags) const {

    if (filter == NULL){
        changeTarget(this, where);

        /*
           TransBlender blender;
           Util::ReferenceCount<Shader> shader;
           if (filter != NULL){
           shader = filter->getShader();
           }

           ALLEGRO_SHADER * a5shader = NULL;
           if (shader != NULL){
           a5shader = shader->getShader();
           }

           if (a5shader != NULL){
           al_set_shader(the_display, a5shader);
           al_use_shader(a5shader, true);
           }

           al_draw_tinted_bitmap(getData()->getBitmap(), getBlendColor().color, x, y, flags);

           if (a5shader != NULL){
           al_set_shader(the_display, shader_default);
           al_use_shader(shader_default, true);
           }
           */

        MaskedBlender blender;

        ALLEGRO_SHADER * shader = shader_lit_sprite->getShader();

        float light[4];
        Color color = makeColor(globalBlend.red, globalBlend.green, globalBlend.blue);
        al_unmap_rgb_f(color.color, &light[0], &light[1], &light[2]);
        light[3] = 1;
        float intensity = (float) globalBlend.alpha / 255.0;
        al_use_shader(shader);
        if (!al_set_shader_float_vector("light_color", 4, light, 1)){
            /* Well.. thats not good. Did the shader source get messed up? */
        }
        if (!al_set_shader_float("light_intensity", intensity)){
        }
        al_draw_bitmap(getData()->getBitmap(), x, y, flags);
        al_use_shader(NULL);
    } else {
        Bitmap temp(getWidth(), getHeight());
        temp.fill(MaskColor());
        Bitmap::draw(0, 0, filter, temp, 0);
        LitBitmap(temp).draw(x, y, NULL, where, flags);
    }
}

void LitBitmap::draw( const int x, const int y, Filter * filter, const Bitmap & where ) const {
    draw(x, y, filter, where, 0);

    // LitBlender blender(makeColorAlpha(globalBlend.red, globalBlend.green, globalBlend.blue, globalBlend.alpha));
    // TransBlender blender;
    // al_draw_bitmap(getData()->getBitmap(), x, y, 0);
    // al_draw_tinted_bitmap(getData()->getBitmap(), al_map_rgba_f(1, 0, 0, 1), x, y, 0);
}

void LitBitmap::drawHFlip( const int x, const int y, const Bitmap & where ) const {
    draw(x, y, NULL, where, ALLEGRO_FLIP_HORIZONTAL);
}

void LitBitmap::drawHFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
    draw(x, y, filter, where, ALLEGRO_FLIP_HORIZONTAL);
}

void LitBitmap::drawVFlip( const int x, const int y, const Bitmap & where ) const {
    draw(x, y, NULL, where, ALLEGRO_FLIP_VERTICAL);
}

void LitBitmap::drawVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
    draw(x, y, filter, where, ALLEGRO_FLIP_VERTICAL);
}

void LitBitmap::drawHVFlip( const int x, const int y, const Bitmap & where ) const {
    draw(x, y, NULL, where, ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL);
}

void LitBitmap::drawHVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
    draw(x, y, filter, where, ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL);
}

void TranslucentBitmap::draw( const int x, const int y, Filter * filter, const Bitmap & where ) const {
    draw(x, y, filter, where, 0);
}

void TranslucentBitmap::drawHFlip( const int x, const int y, const Bitmap & where ) const {
    draw(x, y, NULL, where, ALLEGRO_FLIP_HORIZONTAL);
}

void TranslucentBitmap::drawHFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
    draw(x, y, filter, where, ALLEGRO_FLIP_HORIZONTAL);
}

void TranslucentBitmap::drawVFlip( const int x, const int y, const Bitmap & where ) const {
    draw(x, y, NULL, where, ALLEGRO_FLIP_VERTICAL);
}

void TranslucentBitmap::drawVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
    draw(x, y, filter, where, ALLEGRO_FLIP_VERTICAL);
}

void TranslucentBitmap::drawHVFlip( const int x, const int y, const Bitmap & where ) const {
    draw(x, y, NULL, where, ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL);
}

void TranslucentBitmap::drawHVFlip( const int x, const int y, Filter * filter,const Bitmap & where ) const {
    draw(x, y, filter, where, ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL);
}

void TranslucentBitmap::hLine( const int x1, const int y, const int x2, const Color color ) const {
    TransBlender blender;
    Bitmap::hLine(x1, y, x2, doTransBlend(color, globalBlend.alpha));
}

void TranslucentBitmap::circleFill(int x, int y, int radius, Color color) const {
    TransBlender blender;
    Bitmap::circleFill(x, y, radius, doTransBlend(color, globalBlend.alpha));
}

void TranslucentBitmap::putPixelNormal(int x, int y, Color color) const {
    TransBlender blender;
    Bitmap::putPixelNormal(x, y, doTransBlend(color, globalBlend.alpha));
}

void TranslucentBitmap::rectangle( int x1, int y1, int x2, int y2, Color color ) const {
    TransBlender blender;
    Bitmap::rectangle(x1, y1, x2, y2, doTransBlend(color, globalBlend.alpha));
}

void TranslucentBitmap::rectangleFill(int x1, int y1, int x2, int y2, Color color) const {
    TransBlender blender;
    Bitmap::rectangleFill(x1, y1, x2, y2, doTransBlend(color, globalBlend.alpha));
}

void TranslucentBitmap::ellipse( int x, int y, int rx, int ry, Color color ) const {
    TransBlender blender;
    Bitmap::ellipse(x, y, rx, ry, doTransBlend(color, globalBlend.alpha));
}

void TranslucentBitmap::ellipseFill( int x, int y, int rx, int ry, Color color ) const {
    TransBlender blender;
    Bitmap::ellipseFill(x, y, rx, ry, doTransBlend(color, globalBlend.alpha));
}

void Bitmap::setClipRect( int x1, int y1, int x2, int y2 ) const {
    /* TODO */
}

void Bitmap::getClipRect(int & x1, int & y1, int & x2, int & y2) const {
    /* TODO */
}

int setGfxModeFullscreen(int x, int y){
    return setGraphicsMode(FULLSCREEN, x, y);
}

int setGfxModeWindowed( int x, int y ){
    return setGraphicsMode(WINDOWED, x, y);
}

int setGfxModeText(){
    /* TODO */
    return 0;
}

bool Bitmap::getError(){
    /* TODO */
    return false;
}

void Bitmap::alphaBlender(int source, int dest){
    /* TODO */
}

void Bitmap::transBlender(int r, int g, int b, int a){
    globalBlend.red = r;
    globalBlend.green = g;
    globalBlend.blue = b;
    globalBlend.alpha = a;
    globalBlend.type = Translucent;
}

void Bitmap::addBlender(int r, int g, int b, int a){
    globalBlend.red = r;
    globalBlend.green = g;
    globalBlend.blue = b;
    globalBlend.alpha = a;
    globalBlend.type = Add;
}

void Bitmap::differenceBlender( int r, int g, int b, int a ){
    globalBlend.red = r;
    globalBlend.green = g;
    globalBlend.blue = b;
    globalBlend.alpha = a;
    globalBlend.type = Difference;
}

void Bitmap::multiplyBlender( int r, int g, int b, int a ){
    globalBlend.red = r;
    globalBlend.green = g;
    globalBlend.blue = b;
    globalBlend.alpha = a;
    globalBlend.type = Multiply;
}

/*
void Bitmap::drawingMode(int type){
}
*/

void Bitmap::shutdown(){
    /* Make sure the display is set */
    if (Screen != NULL){
        al_set_target_bitmap(Screen->getData()->getBitmap());
    }
    al_use_shader(NULL);
    shaders.clear();
    shader_shadow = NULL;
    shader_lit_sprite = NULL;
    /*
    for (std::vector<ALLEGRO_SHADER*>::iterator it = shaders.begin(); it != shaders.end(); it++){
        ALLEGRO_SHADER * shader = *it;
        al_destroy_shader(shader);
    }
    */

    delete Screen;
    Screen = NULL;

    al_destroy_display(the_display);
    the_display = NULL;
    /*
    delete Scaler;
    Scaler = NULL;
    delete Buffer;
    Buffer = NULL;
    */
}

StretchedBitmap::StretchedBitmap(int width, int height, const Bitmap & parent, Clear clear, QualityFilter filter):
Bitmap(parent, 0, 0, parent.getWidth(), parent.getHeight()),
width(width),
height(height),
where(parent),
filter(filter),
clearKind(clear){
    scale_x = (double) parent.getWidth() / width;
    scale_y = (double) parent.getHeight() / height;
    ALLEGRO_BITMAP * old_target = al_get_target_bitmap();
    if (al_get_target_bitmap() != parent.getData()->getBitmap()){
        al_set_target_bitmap(parent.getData()->getBitmap());
    }
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    if (al_get_current_transform() != NULL){
        al_copy_transform(&transform, al_get_current_transform());
    }
    al_scale_transform(&transform, scale_x, scale_y);
    if (al_get_target_bitmap() != getData()->getBitmap()){
        al_set_target_bitmap(getData()->getBitmap());
    }
    al_use_transform(&transform);
    if (old_target != al_get_target_bitmap()){
        al_set_target_bitmap(old_target);
    }

    switch (clear){
        case NoClear: break;
        case FullClear: this->clear();
        case Mask: break;
    }

    /* TODO: handle filter */
}

void StretchedBitmap::start(){
#if 0
    ALLEGRO_TRANSFORM transform;
    changeTarget(this, this);
    al_copy_transform(&transform, al_get_current_transform());
    // al_identity_transform(&transform);
    // al_scale_transform(&transform, Bitmap::getWidth() / width, Bitmap::getHeight() / height);
    al_scale_transform(&transform, scale_x, scale_y);
    al_use_transform(&transform);
#endif
}

void StretchedBitmap::finish(){
#if 0
    ALLEGRO_TRANSFORM transform;
    changeTarget(this, this);
    al_copy_transform(&transform, al_get_current_transform());
    /* apply the inverse transform */
    al_scale_transform(&transform, 1.0/scale_x, 1.0/scale_y);
    // al_identity_transform(&transform);
    al_use_transform(&transform);
#endif
}
    
SubTranslucentBitmap::SubTranslucentBitmap(const Bitmap & where, int x, int y, int width, int height, Clear clear):
Bitmap(where, x, y, width, height),
translucent(*this),
clearKind(clear){
    switch (clear){
        case NoClear: break;
        case FullClear: translucent.clear();
        case Mask: break;
    }
}
    
void SubTranslucentBitmap::finish(){
}

void SubTranslucentBitmap::start(){
}
    
void SubTranslucentBitmap::roundRect(int radius, int x1, int y1, int x2, int y2, Color color) const {
    translucent.roundRect(radius, x1, y1, x2, y2, color);
}

void SubTranslucentBitmap::roundRectFill(int radius, int x1, int y1, int x2, int y2, Graphics::Color color) const {
    translucent.roundRectFill(radius, x1, y1, x2, y2, color);
}

TranslatedBitmap::TranslatedBitmap(int x, int y, const Bitmap & where):
Bitmap(where),
x(x),
y(y){
    ALLEGRO_TRANSFORM transform;
    changeTarget(this, where);
    al_identity_transform(&transform);
    if (al_get_current_transform() != NULL){
        al_copy_transform(&transform, al_get_current_transform());
    }
    al_translate_transform(&transform, x, y);
    al_use_transform(&transform);
}

void TranslatedBitmap::BlitToScreen() const {
    Bitmap::BlitToScreen();
}

TranslatedBitmap::~TranslatedBitmap(){
    ALLEGRO_TRANSFORM transform;
    al_copy_transform(&transform, al_get_current_transform());
    al_translate_transform(&transform, -x, -y);
    al_use_transform(&transform);
}

Bitmap * getScreenBuffer(){
    return Screen;
}

RestoreState::RestoreState(){
    al_store_state(&state, ALLEGRO_STATE_ALL);
}

RestoreState::~RestoreState(){
    al_restore_state(&state);
}

Shader::Shader():
shader(NULL){
}

Shader::~Shader(){
    if (shader != NULL){
        al_destroy_shader(shader);
    }
}

Shader::Shader(ALLEGRO_SHADER * shader):
shader(shader){
}

ALLEGRO_SHADER * Shader::getShader(){
    return shader;
}

}

static inline bool close(float x, float y){
    static float epsilon = 0.001;
    return fabs(x - y) < epsilon;
}

static inline bool sameColor(const ALLEGRO_COLOR & color1, const ALLEGRO_COLOR & color2){
    // return memcmp(&color1, &color2, sizeof(Graphics::Color)) == 0;
    float r1, g1, b1, a1;
    float r2, g2, b2, a2;
    al_unmap_rgba_f(color1, &r1, &g1, &b1, &a1);
    al_unmap_rgba_f(color2, &r2, &g2, &b2, &a2);
    return close(r1, r2) &&
           close(g1, g2) &&
           close(b1, b2) &&
           close(a1, a2);

    /*
    unsigned char r1, g1, b1, a1;
    unsigned char r2, g2, b2, a2;
    al_unmap_rgba(color1, &r1, &g1, &b1, &a1);
    al_unmap_rgba(color2, &r2, &g2, &b2, &a2);
    return r1 == r2 &&
           g1 == g2 &&
           b1 == b2 &&
           a1 == a2;
    */
}

static uint32_t quantify(const ALLEGRO_COLOR & color){
    unsigned char red, green, blue, alpha;
    al_unmap_rgba(color, &red, &green, &blue, &alpha);
    return (red << 24) |
           (green << 16) |
           (blue << 8) |
           alpha;
}

bool operator<(const ALLEGRO_COLOR & color1, const ALLEGRO_COLOR & color2){
    return quantify(color1) < quantify(color2);
}

bool operator!=(const ALLEGRO_COLOR & color1, const ALLEGRO_COLOR & color2){
    return !(color1 == color2);
}

bool operator==(const ALLEGRO_COLOR & color1, const ALLEGRO_COLOR & color2){
    return sameColor(color1, color2);
}
