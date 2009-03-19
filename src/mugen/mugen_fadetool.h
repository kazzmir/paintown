#ifndef mugen_fadetool_h
#define mugen_fadetool_h

/*! FADE UTILITY */

class Bitmap;

enum FadeType{
    FADEIN = 0,
    FADEOUT,
    NOFADE,
    RUNFADE
};

class MugenFadeTool{
public:
	MugenFadeTool();
	virtual ~MugenFadeTool();
	
	// Fade state
	void setState(FadeType f);
	
	// update
	void act();
	
	// Fade to whatever state it is at
	virtual void draw(Bitmap *bmp);
	
	inline const FadeType getState() const { return currentState; }
	inline void setFadeInTime(const int time){ fadeInTime = time; }
	inline void setFadeOutTime(const int time){ fadeOutTime = time; }
	inline const int getFadeInTime() const { return fadeInTime; }
	inline const int getFadeOutTime() const { return fadeOutTime; }
	inline void setFadeInColor( int c ){ fadeInColor = c; }
	inline const int getFadeInColor() const { return fadeInColor; }
	inline void setFadeOutColor( int c ){ fadeInColor = c; }
	inline const int getFadeOutColor() const { return fadeInColor; }
    private:
	
	FadeType currentState;
	FadeType lastState;
	int fader;
	int fadeInTime;
	int fadeOutTime;
	int fadeInColor;
	int fadeOutColor;
	
};

#endif
