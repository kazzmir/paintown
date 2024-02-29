#include "r-tech1/debug.h"
#include "r-tech1/system.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <chrono>

#ifdef ANDROID
#include <android/log.h>
#define ANDROID_LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "r-tech1", __VA_ARGS__)
#endif

#if defined(GAMECUBE) || defined(WII)
#include <ogc/system.h>
#endif

#ifdef NETWORK_DEBUG
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#endif

using namespace std;

static int global_debug_level = 0;
static std::string global_log_file = "r-tech1.log";

namespace Global{
#ifdef ANDROID
android_ostream::android_ostream(bool enabled):
enabled(enabled){
}

android_ostream & operator<<(android_ostream & stream, const std::string & input){
    stream.buffer << input;
    return stream;
}

android_ostream & operator<<(android_ostream & stream, const char * input){
    stream.buffer << input;
    return stream;
}

android_ostream & operator<<(android_ostream & stream, const char input){
    stream.buffer << input;
    return stream;
}

android_ostream & operator<<(android_ostream & stream, const double input){
    stream.buffer << input;
    return stream;
}

android_ostream & operator<<(android_ostream & stream, const int input){
    stream.buffer << input;
    return stream;
}

android_ostream & operator<<(android_ostream & stream, const short int input){
    stream.buffer << input;
    return stream;
}

android_ostream & operator<<(android_ostream & stream, const short unsigned int input){
    stream.buffer << input;
    return stream;
}

android_ostream & operator<<(android_ostream & stream, const unsigned int input){
    stream.buffer << input;
    return stream;
}

android_ostream & operator<<(android_ostream & stream, const bool input){
    stream.buffer << input;
    return stream;
}

android_ostream & operator<<(android_ostream & stream, const long int input){
    stream.buffer << input;
    return stream;
}

android_ostream & operator<<(android_ostream & stream, const unsigned long int input){
    stream.buffer << input;
    return stream;
}

android_ostream & operator<<(android_ostream & stream, const uint64_t input){
    stream.buffer << input;
    return stream;
}

android_ostream & operator<<(android_ostream & stream, const void * input){
    stream.buffer << input;
    return stream;
}

android_ostream & operator<<(android_ostream & stream, std::ostream & (*f)(std::ostream &)){
    if (stream.enabled){
        ANDROID_LOGV("%s\n", stream.buffer.str().c_str());
    }
    stream.buffer.str("");
    stream.buffer.rdbuf()->pubseekoff(0, ios_base::end, ios_base::out);
    stream.buffer.clear();
    return stream;
}

android_ostream android_ostream::stream;
static android_ostream nullcout(false);
#elif defined(DEVKITPRO) && defined(DEBUG)
wii_ostream::wii_ostream(bool enabled):
enabled(enabled){
}

wii_ostream & operator<<(wii_ostream & stream, const std::string & input){
    stream.buffer << input;
    return stream;
}

wii_ostream & operator<<(wii_ostream & stream, const char * input){
    stream.buffer << input;
    return stream;
}

wii_ostream & operator<<(wii_ostream & stream, const char input){
    stream.buffer << input;
    return stream;
}

wii_ostream & operator<<(wii_ostream & stream, const double input){
    stream.buffer << input;
    return stream;
}

wii_ostream & operator<<(wii_ostream & stream, const int input){
    stream.buffer << input;
    return stream;
}

wii_ostream & operator<<(wii_ostream & stream, uint64_t input){
    stream.buffer << input;
    return stream;
}

wii_ostream & operator<<(wii_ostream & stream, const short int input){
    stream.buffer << input;
    return stream;
}

wii_ostream & operator<<(wii_ostream & stream, const short unsigned int input){
    stream.buffer << input;
    return stream;
}

wii_ostream & operator<<(wii_ostream & stream, const unsigned int input){
    stream.buffer << input;
    return stream;
}

wii_ostream & operator<<(wii_ostream & stream, const bool input){
    stream.buffer << input;
    return stream;
}

wii_ostream & operator<<(wii_ostream & stream, const long int input){
    stream.buffer << input;
    return stream;
}

#ifndef SWITCH
wii_ostream & operator<<(wii_ostream & stream, const unsigned long int input){
    stream.buffer << input;
    return stream;
}
#endif

wii_ostream & operator<<(wii_ostream & stream, const void * input){
    stream.buffer << input;
    return stream;
}

wii_ostream & operator<<(wii_ostream & stream, std::ostream & (*f)(std::ostream &)){
    if (stream.enabled){
        printf("%s\n", stream.buffer.str().c_str());
#if defined(GAMECUBE) || defined(WII)
        SYS_Report("%s\n", stream.buffer.str().c_str());
        fprintf(stdout, "%s\n", stream.buffer.str().c_str());
        fprintf(stderr, "%s\n", stream.buffer.str().c_str());
#endif
    }
    stream.buffer.str("");
    stream.buffer.rdbuf()->pubseekoff(0, ios_base::end, ios_base::out);
    stream.buffer.clear();
    return stream;
}

wii_ostream wii_ostream::stream;
static wii_ostream nullcout(false);
#elif defined(NETWORK_DEBUG)
network_ostream::network_ostream(const std::string & host, int port, bool enabled):
host(host),
port(port),
enabled(enabled){
}

network_ostream & operator<<(network_ostream & stream, const std::string & input){
    stream.buffer << input;
    return stream;
}

network_ostream & operator<<(network_ostream & stream, const char * input){
    stream.buffer << input;
    return stream;
}

network_ostream & operator<<(network_ostream & stream, const char input){
    stream.buffer << input;
    return stream;
}

network_ostream & operator<<(network_ostream & stream, const double input){
    stream.buffer << input;
    return stream;
}

network_ostream & operator<<(network_ostream & stream, const int input){
    stream.buffer << input;
    return stream;
}

network_ostream & operator<<(network_ostream & stream, uint64_t input){
    stream.buffer << input;
    return stream;
}

network_ostream & operator<<(network_ostream & stream, const short int input){
    stream.buffer << input;
    return stream;
}

network_ostream & operator<<(network_ostream & stream, const short unsigned int input){
    stream.buffer << input;
    return stream;
}

network_ostream & operator<<(network_ostream & stream, const unsigned int input){
    stream.buffer << input;
    return stream;
}

network_ostream & operator<<(network_ostream & stream, const bool input){
    stream.buffer << input;
    return stream;
}

network_ostream & operator<<(network_ostream & stream, const long int input){
    stream.buffer << input;
    return stream;
}

#ifndef PS3
network_ostream & operator<<(network_ostream & stream, const unsigned long int input){
    stream.buffer << input;
    return stream;
}
#endif

network_ostream & operator<<(network_ostream & stream, const void * input){
    stream.buffer << input;
    return stream;
}

static void sendString(const std::string & host, int port, const std::string & data){
    int gateway = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(host.c_str());
    memset(address.sin_zero, '\0', sizeof(address.sin_zero));
    connect(gateway, (struct sockaddr*) &address, sizeof(address));
    send(gateway, data.c_str(), data.size(), 0);
    send(gateway, "\n", 1, 0);
    close(gateway);
}

network_ostream & operator<<(network_ostream & stream, std::ostream & (*f)(std::ostream &)){
    if (stream.enabled){
        sendString(stream.host, stream.port, stream.buffer.str());
        // printf("%s\n", stream.buffer.str().c_str());
    }
    stream.buffer.str("");
    stream.buffer.rdbuf()->pubseekoff(0, ios_base::end, ios_base::out);
    stream.buffer.clear();
    return stream;
}

/* IP address and port are arbitrary */
network_ostream network_ostream::stream("192.168.1.100", 5670);
static network_ostream nullcout("", 0, false);
#else
class nullstreambuf_t: public std::streambuf {
public:
    nullstreambuf_t():std::streambuf(){
    }
};

static nullstreambuf_t nullstreambuf;

class nullcout_t: public std::ostream {
public:
    nullcout_t():std::ostream(&nullstreambuf){
    }
};

static nullcout_t nullcout;
#endif

#ifdef ANDROID
static stream_type & defaultStream(){
    return android_ostream::stream;
}

static stream_type & getStream(){
    return defaultStream();
}

void logToFile(){
}

void closeLog(){
}
#elif defined(DEVKITPRO) && defined(DEBUG)
static stream_type & defaultStream(){
    return wii_ostream::stream;
}

static stream_type & getStream(){
    return defaultStream();
}

void logToFile(){
}

void closeLog(){
}
#elif defined(NETWORK_DEBUG)
static stream_type & defaultStream(){
    return network_ostream::stream;
}

static stream_type & getStream(){
    return defaultStream();
}

void logToFile(){
}

void closeLog(){
}
#else
static stream_type & defaultStream(){
    return std::cout;
}

static std::ofstream log;
static bool useFile = false;
static stream_type & fileStream(){
    static bool init = false;
    if (!init){
        log.open(Global::getLogFile().c_str());
        init = true;
    }

    return log;
}

static void closeFileStream(){
    log.close();
}

static stream_type & getStream(){
    if (useFile){
        return fileStream();
    } else {
        return defaultStream();
    }
}

void logToFile(){
    useFile = true;
}

void closeLog(){
    closeFileStream();
}
#endif

}

Global::stream_type & Global::debug(int i, const string & context){
    if (global_debug_level >= i){
        Global::stream_type & out = getStream();
        out << "[" << i << ":" << context << "] ";
        return out;
    }
    return nullcout;
}

void Global::setDebug(int i){
    global_debug_level = i;

#if defined(GAMECUBE) || defined(WII)
    // Redirects stderr and stdio to Dolphin OSReport uart
    SYS_STDIO_Report(true);
#endif
}

int Global::getDebug(){
    return global_debug_level;
}

void Global::setLogFile(const std::string & file){
    global_log_file = file;
}

const std::string & Global::getLogFile(){
    return global_log_file;
}

std::string Global::defaultDebugContext = "default";

void Global::setDefaultDebugContext(const std::string & context){
    defaultDebugContext = context;
}


/* FIXME */
static std::string now(){
    /* Replace with chrono */

    time_t t = time(NULL);
    tm * timeData;
    timeData = localtime(&t);
    uint64_t millis = System::currentMilliseconds() % 1000;
    if (timeData != NULL){
        char buffer[200];
        strftime(buffer, sizeof(buffer), "%F %I:%M:%S", timeData);
        std::ostringstream out;
        out << buffer << "." << millis;
        strftime(buffer, sizeof(buffer), " %p", timeData);
        out << buffer;
        return out.str();
        // return std::string(buffer);
    }

    std::ostringstream out;
#if 0
    auto time = std::chrono::system_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch());
    char buffer[200];
    //strftime(buffer, sizeof(buffer), "%F %I:%M:%S", timeData);
    std::ostringstream out;
    out << buffer << "." << millis.count();
    //strftime(buffer, sizeof(buffer), " %p", timeData);
    out << buffer;
    return out.str();
#endif 
    return out.str();
}

std::string Global::debug_context(const char * file, int line){
    std::ostringstream out;
    out << file << ":" << line << " " << now();
    return out.str();
}
