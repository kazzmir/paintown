#ifndef _paintown_mugen_common_h
#define _paintown_mugen_common_h

/* FIXME: we only need this header to get Graphics::Bitmap::Filter but we can't
 * just forward declare Filter because Bitmap is a class not a namespace.
 * Move Filter to the Graphics namespace so we can forward declare it here
 * and get rid of this include.
 */
#include "util/graphics/bitmap.h"

namespace Mugen{

class Stage;
class Character;

enum Facing{
    FacingLeft, FacingRight
};

/* Wrapper class for a character id (int) */
class CharacterId{
private:
    int id;

public:
    CharacterId();
    CharacterId(const CharacterId & copy);
    explicit CharacterId(int value);
    virtual ~CharacterId();
    bool operator==(const CharacterId &) const;
    bool operator!=(const CharacterId &) const;
    bool operator<(const CharacterId &) const;

    /* Used to pass the raw integer value to mugen script */
    int intValue() const;
};

namespace Physics{

enum Type{
    None, /* N */
    Air, /* A */
    Stand, /* S */
    Crouch, /* C */
    Unchanged /* U */
};

}

namespace AttackType{
    extern std::string Normal;
    extern std::string Special;
    extern std::string Hyper;

    enum Attribute{
        NoAttribute,
        NormalAttack,
        NormalThrow,
        NormalProjectile,
        SpecialAttack,
        SpecialThrow,
        SpecialProjectile,
        HyperAttack,
        HyperThrow,
        HyperProjectile
    };

    enum Animation{
        NoAnimation = -1,
        Light = 0,
        Medium = 1,
        Hard = 2,
        Back = 3,
        Up = 4,
        DiagonalUp = 5
    };

    /* Badly named.. */
    enum Ground{
        None = 0,
        High,
        Low,
        Trip
    };
}

enum TransType{
    None = 0,
    Add,
    Add1,
    Sub,
    AddAlpha,
    Translucent,
    /* used by the Trans controller to specify the trans should not change */
    Default
};

class Effects{
    public:
	Effects();
        Effects(const Effects & copy);
	const Effects &operator=(const Effects &e);
	virtual ~Effects();

        /* adds the mask from the parameter to this object */
        Effects operator+(const Effects &) const;

        /* adds the translucency */
        Effects operator+(const TransType & translucent) const;

        /* what kind of translucency to use */
	TransType trans;

        /* translucency values */
	int alphaSource;
	int alphaDest;

        /* if the masking color should be shown or not, true is not shown */
	bool mask;

        /* horizontal flip. true for flip, false for no flip */
	bool facing;

        /* vertical flip */
	bool vfacing;

        /* scale horizontally */
	double scalex;

        /* scale vertically */
	double scaley;

        double rotation;

        /* clipping. -1 means dont alter clipping */
        struct Dimension{
            Dimension():x1(-1), x2(-1), y1(-1), y2(-1){}

            int x1, x2;
            int y1, y2;
        } dimension;

        Graphics::Bitmap::Filter * filter;
};

struct Constant{
    enum ConstantType{
        None,
        Double,
        ListOfDouble,
    };

    Constant():
    type(None){
    }

    Constant(double value):
    type(Double),
    double_(value){
    }

    Constant(std::vector<double> doubles):
    type(ListOfDouble),
    doubles(doubles){
    }

    ConstantType type;

    double double_;
    std::vector<double> doubles;
};

struct WinGame{
    /* TODO: add an explanation for each win type that describes how to
     * achieve that state.
     */
    enum WinType{
        Normal,
        Special,
        Hyper,
        NormalThrow,
        Cheese,
        TimeOver,
        Suicide,
        Teammate,
        /* Overlayed */
        /* is this needed now that the `perfect' bool exists? */
        Perfect,
    };

    WinGame():
    type(Normal),
    perfect(false){
    }

    WinType type;
    bool perfect;

};

}

#endif
