#ifndef _rtech1_debug_h
#define _rtech1_debug_h

#include <ostream>
#include <sstream>
#include <stdint.h>

#define PAINTOWN_DEBUG_CONTEXT Global::debug_context(__FILE__, __LINE__)
#define _xdebug Global::debug(0, PAINTOWN_DEBUG_CONTEXT)

/* Enable this if you can't get regular debug output but have networking
 */
// #define NETWORK_DEBUG

namespace Global{

#ifdef ANDROID
class android_ostream: public std::ostream {
public:
    android_ostream(bool enabled = true);
    static android_ostream stream;
    /* make these private at some point */
public:
    bool enabled;
    std::ostringstream buffer;
};

typedef android_ostream stream_type;
android_ostream & operator<<(android_ostream & stream, const std::string & input);
android_ostream & operator<<(android_ostream & stream, const char * input);
android_ostream & operator<<(android_ostream & stream, const char);
android_ostream & operator<<(android_ostream & stream, const double);
android_ostream & operator<<(android_ostream & stream, const int);
android_ostream & operator<<(android_ostream & stream, const short int);
android_ostream & operator<<(android_ostream & stream, const short unsigned int);
android_ostream & operator<<(android_ostream & stream, const unsigned int);
android_ostream & operator<<(android_ostream & stream, const bool);
android_ostream & operator<<(android_ostream & stream, const long int);
android_ostream & operator<<(android_ostream & stream, const unsigned long int);
android_ostream & operator<<(android_ostream & stream, const uint64_t);
android_ostream & operator<<(android_ostream & stream, const void *);
android_ostream & operator<<(android_ostream & stream, const double);
android_ostream & operator<<(android_ostream & stream, std::ostream & (*f)(std::ostream &));
#elif defined(DEVKITPRO) && defined(DEBUG)
class wii_ostream: public std::ostream {
public:
    wii_ostream(bool enabled = true);
    static wii_ostream stream;
    /* make these private at some point */
public:
    bool enabled;
    std::ostringstream buffer;
};

typedef wii_ostream stream_type;
wii_ostream & operator<<(wii_ostream & stream, const std::string & input);
wii_ostream & operator<<(wii_ostream & stream, const char * input);
wii_ostream & operator<<(wii_ostream & stream, const char);
wii_ostream & operator<<(wii_ostream & stream, const double);
wii_ostream & operator<<(wii_ostream & stream, const int);
wii_ostream & operator<<(wii_ostream & stream, const short int);
wii_ostream & operator<<(wii_ostream & stream, const short unsigned int);
wii_ostream & operator<<(wii_ostream & stream, const unsigned int);
wii_ostream & operator<<(wii_ostream & stream, const bool);
wii_ostream & operator<<(wii_ostream & stream, const long int);
wii_ostream & operator<<(wii_ostream & stream, const unsigned long int);
wii_ostream & operator<<(wii_ostream & stream, const void *);
wii_ostream & operator<<(wii_ostream & stream, uint64_t); 
wii_ostream & operator<<(wii_ostream & stream, std::ostream & (*f)(std::ostream &));
#elif defined(NETWORK_DEBUG)
class network_ostream: public std::ostream {
public:
    network_ostream(const std::string & host, int port, bool enabled = true);
    static network_ostream stream;
    /* make these private at some point */
public:
    std::string host;
    int port;
    bool enabled;
    std::ostringstream buffer;
};

typedef network_ostream stream_type;
stream_type & operator<<(stream_type & stream, const std::string & input);
stream_type & operator<<(stream_type & stream, const char * input);
stream_type & operator<<(stream_type & stream, const char);
stream_type & operator<<(stream_type & stream, const double);
stream_type & operator<<(stream_type & stream, const int);
stream_type & operator<<(stream_type & stream, const short int);
stream_type & operator<<(stream_type & stream, const short unsigned int);
stream_type & operator<<(stream_type & stream, const unsigned int);
stream_type & operator<<(stream_type & stream, const bool);
stream_type & operator<<(stream_type & stream, const long int);
#ifndef PS3
stream_type & operator<<(stream_type & stream, const unsigned long int);
#endif
stream_type & operator<<(stream_type & stream, const void *);
stream_type & operator<<(stream_type & stream, uint64_t); 
stream_type & operator<<(stream_type & stream, std::ostream & (*f)(std::ostream &));

#else
typedef std::ostream stream_type;
#endif

void logToFile();
void closeLog();
void setDebug(int i);
int getDebug();
void setLogFile(const std::string &);
const std::string & getLogFile();
extern std::string defaultDebugContext;
void setDefaultDebugContext(const std::string & context);
stream_type & debug(int i, const std::string & context = defaultDebugContext);
std::string debug_context(const char * file, int line);

}

#endif
