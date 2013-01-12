#ifndef _paintown_mugen_stage_state_h
#define _paintown_mugen_stage_state_h

#include "common.h"

namespace Mugen{

    struct ScreenBound{
        ScreenBound():
            enabled(false),
            offScreen(false),
            panX(false),
            panY(false){
            }

        bool enabled;
        bool offScreen;
        bool panX;
        bool panY;
    };
  
    struct StageStateData{
        StageStateData();

        struct Pause{
            Pause():
                time(0){
                }

            int time;
            int buffer;
            int moveTime;
            bool pauseBackground;
            CharacterId who;
        } pause;

        /* Screen bound is per character so make a map from character to their
         * ScreenBound data
         */
        std::map<CharacterId, ScreenBound> screenBound;

        /* For zooming in on a character/position */
        struct Zoom{
            Zoom():
                enabled(false){
                }

            bool enabled;
            double x;
            double y;
            int zoomTime;
            int zoomOutTime;

            /* keeps track of the current zoom time and whether its going in or out */
            int zoom;
            bool in;

            int time;
            int bindTime;

            /* keeps track of the relative position between ... */
            int deltaX, deltaY;

            /* scaling factor. must be at least 1
            */
            double scaleX;
            double scaleY;

            double velocityX;
            double velocityY;
            double accelX;
            double accelY;
            int superMoveTime;
            int pauseMoveTime;
            bool removeOnGetHit;
            int hitCount;
            /* Character we are bound to, depending on the original postype */
            CharacterId bound;

            /* Character that started the zoom in case they get hit */
            CharacterId owner;
        } zoom;

        struct EnvironmentColor{
            EnvironmentColor():
                time(0),
                under(false){
                }

            Graphics::Color color;
            int time;
            bool under;
        } environmentColor;

        struct SuperPause{
            SuperPause():time(0){
            }

            int time;
            int positionX, positionY;
            int soundGroup, soundItem;
        } superPause;

        int quake_time;
        int cycles;

        // This is for when characters are inside the tension area
        int inleft;
        int inright;
        int onLeftSide;
        int onRightSide;
        int inabove;

        // Camera location on stage
        double camerax;
        double cameray;

        // Our ticker/frames during duration of the entire stage lifetime
        unsigned long int ticker;
    };
}

#endif
