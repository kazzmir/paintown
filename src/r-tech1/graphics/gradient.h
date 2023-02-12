#ifndef _rtech1_gradient_h
#define _rtech1_gradient_h

#include "bitmap.h"
#include <stdint.h>

class Token;

namespace Effects{

class Gradient{
public:
    Gradient();
    Gradient(Graphics::Color singleColor);
    Gradient(int size, Graphics::Color startColor, Graphics::Color endColor);
    Gradient(const Gradient & copy);
    Gradient(const Token * token);

    Gradient & operator=(const Gradient & copy);

    /* move to next color. update is an alias for `forward' */
    void update();
    void forward();
    void backward();

    /* start at startColor */
    void reset();

    /* get current color */
    Graphics::Color current() const;
    Graphics::Color current(int offset) const;

    virtual ~Gradient();

protected:
    Graphics::Color * colors;
    uint32_t size;
    uint32_t index;
};

}

#endif
