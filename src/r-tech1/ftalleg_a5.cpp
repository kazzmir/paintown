#include "ftalleg.h"

freetype::freetype(const Filesystem::AbsolutePath & path, const int x, const int y):
    alive(5, 5),
    path(path),
    width(x),
    height(y),
    original_size(x){
        if (instances == 0){
            al_init_font_addon();
            al_init_ttf_addon();
        }
        instances += 1;

        int flags = al_get_new_bitmap_flags();
        /* memory fonts must live in memory */
        al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
        fonts[width].memory = al_load_font(path.path().c_str(), width, 0);
        al_set_new_bitmap_flags(flags);
        fonts[width].video = al_load_font(path.path().c_str(), width, 0);
    }

    freetype::~freetype(){
        for (std::map<int, FontUse>::iterator it = fonts.begin(); it != fonts.end(); it++){
            FontUse & font = it->second;
            al_destroy_font(font.memory);
            al_destroy_font(font.video);
        }

        instances -= 1;
        if (instances == 0){
            al_shutdown_font_addon();
        }
    }

    ALLEGRO_FONT * freetype::currentMemoryFont() const {
        std::map<int, FontUse>::const_iterator find = fonts.find(width);
        if (find == fonts.end()){
            throw Exception("inconsistency error");
        }
        return find->second.memory;
    }

    ALLEGRO_FONT * freetype::currentVideoFont() const {
        std::map<int, FontUse>::const_iterator find = fonts.find(width);
        if (find == fonts.end()){
            throw Exception("inconsistency error");
        }
        return find->second.video;
    }
    
    int freetype::getHeight(const std::string & str) const {
        Util::Thread::ScopedLock locked(lock);
        /* sort of a hack but we need to set the display to the screen. with
         * allegro5 the screen buffer will be the actual screen so no allocation
         * will occur.
         */
        // al_set_target_bitmap(alive.getData().getBitmap());
        // ALLEGRO_BITMAP * target = al_get_target_bitmap();
        // al_set_target_bitmap(NULL);
        int height = al_get_font_line_height(currentMemoryFont());
        // al_set_target_bitmap(target);
        return height;
    }

    int freetype::getLength(const std::string & text) const {
        Util::Thread::ScopedLock locked(lock);
        // al_set_target_bitmap(alive.getData().getBitmap());
        // ALLEGRO_BITMAP * target = al_get_target_bitmap();
        // al_set_target_bitmap(NULL);
        int width = al_get_text_width(currentMemoryFont(), text.c_str());
        // al_set_target_bitmap(target);
        return width;
    }
            
    void freetype::setSize(unsigned int w, unsigned int h){
        Util::Thread::ScopedLock locked(lock);
        width = w;
        height = h;
        if (fonts.find(width) == fonts.end()){
            fonts[width].memory = al_load_font(path.path().c_str(), width, 0);
            fonts[width].video = al_load_font(path.path().c_str(), width, 0);
        }
    }

    void freetype::getSize(int * w, int * h) const {
        Util::Thread::ScopedLock locked(lock);
        *w = width;
        *h = height;
    }
            
    void freetype::render(int x, int y, const Graphics::Color & color, const Graphics::Bitmap & bmp, ftAlign alignment, const std::string & text, int marker, ...){
        Util::Thread::ScopedLock locked(lock);
        std::ostringstream str;

        /* use vsnprintf/Util::limitPrintf here? */

        // Get extra arguments
        va_list ap;
        va_start(ap, marker);
        for(unsigned int i = 0; i<text.length();++i) {
            if (text[i] == '%') {
                if(text[i+1]=='s') {
                    str << va_arg(ap, char *);
                    ++i;
                } else if(text[i+1]=='d'||text[i+1]=='i') {
                    str << va_arg(ap, signed int);
                    ++i;
                } else if(text[i+1]=='c') {
                    str << (char)va_arg(ap, int);
                    ++i;
                } else str << text[i];
            } else {
                str << text[i];
            }
        }
        va_end(ap);

        std::string fixedText(str.str());
        if (al_get_target_bitmap() != bmp.getData()->getBitmap()){
            al_set_target_bitmap(bmp.getData()->getBitmap());
        }
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

        /* for setting the blend state and whatnot */
        bmp.startDrawing();
        al_draw_text(currentVideoFont(), bmp.blendColor(color).color, x, y, 0, fixedText.c_str());
        bmp.endDrawing();
    }

