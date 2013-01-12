#ifndef _paintown_mugen_random_h
#define _paintown_mugen_random_h

#include <stdint.h>

namespace Mugen{

/* Uses the WELL 512 random algorithm.
 * http://www.iro.umontreal.ca/~panneton/WELLRNG.html
 */
class Random{
public:
    /* Uses time() for default initialization */
    Random();
    Random(uint32_t seed);
    Random(const Random & copy);

    Random & operator=(const Random &);

    uint64_t next();

protected:
    void init(uint32_t);

    uint64_t state[16];
    uint8_t index;
};

}

#endif
