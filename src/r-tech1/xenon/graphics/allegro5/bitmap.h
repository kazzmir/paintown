#include <allegro5/allegro5.h>

struct BitmapData{
    BitmapData():
    bitmap(NULL),
    destroy(true){
    }

    BitmapData(ALLEGRO_BITMAP * bitmap):
    bitmap(bitmap),
    destroy(true){
    }

    virtual ~BitmapData(){
        if (bitmap != NULL){
            if (al_get_target_bitmap() == bitmap){
                al_set_target_bitmap(NULL);
            }
            if (destroy){
                al_destroy_bitmap(bitmap);
            }
        }
    }

    void setDestroy(bool ok){
        destroy = ok;
    }

    inline ALLEGRO_BITMAP * getBitmap() const {
        return bitmap;
    }

    inline void setBitmap(ALLEGRO_BITMAP * b){
        bitmap = b;
    }

    ALLEGRO_BITMAP * bitmap;
    bool destroy;
};

namespace Graphics{
    typedef ALLEGRO_COLOR INTERNAL_COLOR;
    /* events.cpp needs this */
    extern ALLEGRO_DISPLAY * the_display;

    class RestoreState{
    public:
        RestoreState();
        ~RestoreState();
    protected:
        ALLEGRO_STATE state;
    };
}

bool operator<(const ALLEGRO_COLOR & color1, const ALLEGRO_COLOR & color2);
bool operator!=(const ALLEGRO_COLOR & color1, const ALLEGRO_COLOR & color2);
bool operator==(const ALLEGRO_COLOR & color1, const ALLEGRO_COLOR & color2);
