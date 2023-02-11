#include <string>
#include <stdint.h>

namespace Utf{
    long readUtf8CodePoint(const std::string & input, unsigned int * position);
    std::string utf16_to_utf8(const uint16_t * utf16);
}
