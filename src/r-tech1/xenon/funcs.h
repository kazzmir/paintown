#ifndef _rtech1_funcs_h
#define _rtech1_funcs_h

#include <stdlib.h>
#include <vector>
#include <string>
#include <stdarg.h>

#include "regex.h"
#include "file-system.h"

namespace Exception{
    class Base;
}

namespace Graphics{
class Bitmap;
}

namespace Util{

    extern const double pi;


/* returns a number between 0 and q-1. you will never get `q'. if
 * you wanted to get `q' then pass in q+1
 */
inline int rnd( int q ){
    if (q <= 0){
        return 0;
    }

    return (int)(rand() % q);
}

// std::vector< std::string > getFiles(const Filesystem::AbsolutePath & dataPath, const std::string & find );

double radians(double degree);
double degrees(double radians);

Filesystem::AbsolutePath getDataPath2();

void setDataPath( const std::string & str );

bool exists( const std::string & file );

/* check that `version' is compatible with this program, mostly used
 * for network clients.
 */
// bool checkVersion(int version);

/* return a random number + some range between min/max */
int rnd( int q, int min, int max );

int max(int a, int b);

inline int min(int a, int b){
    if (a<b){
        return a;
    }
    return b;
}

double min(double a, double b);
double max(double a, double b);

double distance(double x1, double y1, double x2, double y2);

int clamp(int value, int min, int max);
double clamp(double value, double min, double max);

/* return a number between min/max */
int rnd( int min, int max );

/* rest in milliseconds */
void rest( int x );

/* rest in seconds */
void restSeconds(double x);

std::string trim(const std::string & str);
/* only upper cases the first letter of a string */
std::string upcase(std::string str);

/* lower cases the entire string */
std::string lowerCaseAll(std::string str);

/* upper cases the entire string */
std::string upperCaseAll(std::string str);

// Split string
std::vector<std::string> splitString(std::string str, char splitter);

// Join strings
std::string joinStrings(const std::vector< std::string > & message, unsigned int start = 0);

/* convert a size in bytes into human readable form.
 * 234823592 = 223.94m
 */
std::string niceSize(unsigned long size);

int levenshtein(const std::string & str1, const std::string & str2);

// int getPipe(int files[2]);

void limitPrintf(char * buffer, int size, const char * format, va_list args);

void showError(const Graphics::Bitmap & screen, const Exception::Base & exception, const std::string & info);
/* will create a screen of size GFX_X, GFX_Y */
void showError(const Exception::Base & exception, const std::string & info);

std::string join(const std::vector<std::string> & list, const std::string & middle);

template <class T, size_t N> size_t array_size(T (&)[N]){ return N; }

}

#endif
