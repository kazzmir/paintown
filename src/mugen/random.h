#ifndef _paintown_mugen_random_h
#define _paintown_mugen_random_h

#include <stdint.h>
#include "util/pointer.h"

namespace PaintownUtil = ::Util;
class Token;

namespace Mugen{

/* Uses the WELL 512 random algorithm.
 * http://www.iro.umontreal.ca/~panneton/WELLRNG.html
 */
class Random{
public:
    /* Uses time() for default initialization */
    Random();
    Random(const Random & copy);

    Random & operator=(const Random &);

    uint64_t next();

    static uint32_t random();

    static PaintownUtil::ReferenceCount<Random> getState();
    static void setState(const Random & what);

    Token * serialize() const;
    static Random deserialize(const Token * token);

protected:
    void init();

    uint64_t state[16];
    uint8_t index;

    static PaintownUtil::ReferenceCount<Random> current;
};

uint32_t random();
uint32_t random(int high);
uint32_t random(int low, int high);

}

#endif
