#include "r-tech1/utf.h"
#include <string>
#include <sstream>

using std::string;
using std::ostringstream;

namespace Utf{

    /* utf8 decoding. reads a utf8 string and returns a unicode code point.
     * position should be a pointer to an integer that stores the current
     *  place in the input string where the next character is to be read.
     * FIXME: handle the case when `position' runs out of bytes
     */
    long readUtf8CodePoint(const std::string & input, unsigned int * position){
        unsigned char byte1 = (unsigned char) input[*position];
        /* one byte - ascii */
        if (byte1 >> 7 == 0){
            return byte1;
        }

        if (byte1 >> 5 == 6){
            *position += 1;
            if (*position >= input.size()){
                return 0;
            }
            unsigned char byte2 = (unsigned char) input[*position];
            int top = byte1 & 31;
            int bottom = byte2 & 63;
            return (top << 6) + bottom;
        }

        if (byte1 >> 4 == 14){
            *position += 1;
            if (*position >= input.size()){
                return 0;
            }
            unsigned char byte2 = (unsigned char) input[*position];
            *position += 1;
            if (*position >= input.size()){
                return 0;
            }
            unsigned char byte3 = (unsigned char) input[*position];
            int top4 = byte1 & 15;
            int middle6 = byte2 & 63;
            int bottom6 = byte3 & 63;
            return (top4 << (6+6)) + (middle6 << 6) + bottom6;
        }

        if (byte1 >> 3 == 30){
            *position += 1;
            if (*position >= input.size()){
                return 0;
            }
            unsigned char byte2 = (unsigned char) input[*position];
            *position += 1;
            if (*position >= input.size()){
                return 0;
            }
            unsigned char byte3 = (unsigned char) input[*position];
            *position += 1;
            if (*position >= input.size()){
                return 0;
            }
            unsigned char byte4 = (unsigned char) input[*position];
            int unit1 = byte1 & 7;
            int unit2 = byte2 & 63;
            int unit3 = byte3 & 63;
            int unit4 = byte4 & 63;
            return (unit1 << 18) + (unit2 << 12) + (unit3 << 6) + unit4;
        }

        return 0;
    }

    string codePointToUtf8(long unicode){
        ostringstream out;
        if (unicode <= 0x7f){
            out << (char) unicode;
        } else if (unicode <= 0x7ff){
            /* 110xxxxx 10xxxxxx */
            unsigned char byte1 = ((unicode >> (1 * 6)) & 0x1f) | (6 << 5);
            unsigned char byte2 = (unicode & 0x3f) | (2 << 6);
            out << byte1 << byte2;
        } else if (unicode <= 0xffff){
            /* 1110xxxx 10xxxxxx 10xxxxxx */
            unsigned char byte1 = ((unicode >> (2 * 6)) & 0xf) | (0xe << 4);
            unsigned char byte2 = ((unicode >> (1 * 6)) & 0x3f) | (2 << 6);
            unsigned char byte3 = (unicode & 0x3f) | (2 << 6);
            out << byte1 << byte2 << byte3;
        } else if (unicode <= 0x1fffff){
            /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
            unsigned char byte1 = ((unicode >> (3 * 6)) & 0x7) | (0x1e << 3);
            unsigned char byte2 = ((unicode >> (2 * 6)) & 0x3f) | (2 << 6);
            unsigned char byte3 = ((unicode >> (1 * 6)) & 0x3f) | (2 << 6);
            unsigned char byte4 = (unicode & 0x3f) | (2 << 6);
            out << byte1 << byte2 << byte3 << byte4;
        } else if (unicode <= 0x3ffffff){
            /* 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
            unsigned char byte1 = ((unicode >> (4 * 6)) & 0x3) | (0x3e << 2);
            unsigned char byte2 = ((unicode >> (3 * 6)) & 0x3f) | (2 << 6);
            unsigned char byte3 = ((unicode >> (2 * 6)) & 0x3f) | (2 << 6);
            unsigned char byte4 = ((unicode >> (1 * 6)) & 0x3f) | (2 << 6);
            unsigned char byte5 = (unicode & 0x3f) | (2 << 6);
            out << byte1 << byte2 << byte3 << byte4 << byte5;
        } else if (unicode <= 0x7fffffff){
            /* 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
            unsigned char byte1 = ((unicode >> (5 * 6)) & 0x1) | (0x7e << 1);
            unsigned char byte2 = ((unicode >> (4 * 6)) & 0x3f) | (2 << 6);
            unsigned char byte3 = ((unicode >> (3 * 6)) & 0x3f) | (2 << 6);
            unsigned char byte4 = ((unicode >> (2 * 6)) & 0x3f) | (2 << 6);
            unsigned char byte5 = ((unicode >> (1 * 6)) & 0x3f) | (2 << 6);
            unsigned char byte6 = (unicode & 0x3f) | (2 << 6);
            out << byte1 << byte2 << byte3 << byte4 << byte5 << byte6;
        }

        return out.str();
    }

    /* Reads a code point from a utf16 string made up of uint16_t integers */
    /* FIXME: add length for input */
    long readUtf16CodePoint(const uint16_t * input, unsigned int * position){
        uint16_t word1 = input[*position];
        *position += 1;

        /*  1) If W1 < 0xD800 or W1 > 0xDFFF, the character value U is the value
         *     of W1. Terminate.
         */
        if (word1 < 0xd800 || word1 > 0xdfff){
            return word1;
        }

        /* 2) Determine if W1 is between 0xD800 and 0xDBFF. If not, the sequence
         *    is in error and no valid character can be obtained using W1.
         *    Terminate.
         */
        if (word1 >= 0xd800 && word1 <= 0xdbff){

            /* 3) If there is no W2 (that is, the sequence ends with W1), or if W2
             *    is not between 0xDC00 and 0xDFFF, the sequence is in error.
             *    Terminate.
             */
            uint16_t word2 = input[*position];
            if (word2 >= 0xdc00 && word2 <= 0xdfff){
                *position += 1;
                /* 4) Construct a 20-bit unsigned integer U', taking the 10 low-order
                 *    bits of W1 as its 10 high-order bits and the 10 low-order bits of
                 *    W2 as its 10 low-order bits.
                 */
                /* 5) Add 0x10000 to U' to obtain the character value U. Terminate. */
                return (((word1 & 0x3ff) << 10) |
                         (word2 & 0x3ff)) + 0x10000;
            }
        }
        
        return 0;
    }
    
    std::string utf16_to_utf8(const uint16_t * utf16){
        ostringstream out;
        unsigned int position = 0;
        while (utf16[position] != 0){
            out << codePointToUtf8(readUtf16CodePoint(utf16, &position));
        }
        return out.str();
    }

}
