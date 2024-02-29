#ifdef USE_ALLEGRO
#include <allegro.h>
#endif
#ifdef USE_SDL
#include <SDL.h>
#endif
#ifdef USE_ALLEGRO5
#include <allegro5/allegro.h>
#endif
#include "r-tech1/funcs.h"
#include "r-tech1/debug.h"
#include <vector>
#include <string>
#include <string.h>
#include <algorithm>
#include "r-tech1/file-system.h"
#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/font.h"
#include "r-tech1/init.h"
#include <sstream>
#include "r-tech1/file-system.h"
#include <math.h>

#ifndef CROSS_BUILD
#define FS_WRAPPER
#include "libs/filesystem/fs-wrapper.h"
//#include "libs/filesystem/glob.h"
#endif

#ifndef USE_ALLEGRO
/* FIXME: move this to the filesystem module */
//#include "libs/sfl/sfl.h"
//#include "libs/sfl/sflfile.h"
#endif

#ifndef WINDOWS
#include <unistd.h>
#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

using namespace std;

/* remove this once cmake and scons properly set DATA_PATH */
#ifndef DATA_PATH
#define DATA_PATH "data"
#endif

/* the build system should define DATA_PATH */
static string dataPath = DATA_PATH;
    
const double Util::pi = 3.14159265358979323846;

double Util::radians(double degree){
    return degree * pi / 180.0;
}

double Util::degrees(double radians){
    return radians * 180.0 / pi;
}

/*
inline int rnd( int q ){
    if ( q <= 0 ) return 0;
    return (int)( rand() % q );
}
*/

int Util::max(int a, int b){
    if (a>b){
        return a;
    }
    return b;
}

double Util::min(double a, double b){
    if (a < b){
        return a;
    }
    return b;
}

double Util::max(double a, double b){
    if (a > b){
        return a;
    }
    return b;
}

/*
int Util::min(int a, int b){
    if (a<b){
        return a;
    }
    return b;
}
*/

int Util::clamp(int value, int min, int max){
    return Util::min(Util::max(value, min), max);
}

double Util::clamp(double value, double min, double max){
    return Util::min(Util::max(value, min), max);
}

int Util::rnd( int q, int min, int range ){
    return q - min + rnd( range );
}

int Util::rnd( int min, int max ){
    return rnd( max - min ) + min;
}

/* sleep for `x' milliseconds */
void Util::rest( int x ){
#ifdef USE_ALLEGRO
    ::rest(x);
#endif
#ifdef USE_SDL
    SDL_Delay(x);
#endif
#ifdef USE_ALLEGRO5
    al_rest((double) x / 1000.0);
#endif
#ifdef USE_SDL2
    SDL_Delay(x);
#endif
}

void Util::restSeconds(double x){
    Util::rest((int)(x * 1000));
}

#if 0
bool Util::checkVersion(int version){
    if (version == Global::getVersion()){
        return true;
    }

    /* when an incompatible version is made, add a check here, like
     *  version < getVersion(3, 5)
     * would mean any client below version 3.5 is incompatible.
     *
     * assume versions of client's greater than ourself is compatible, but
     * this may not be true. There is no way to check this.
     */
    if (version < 0){
        return false;
    }

    return true;
}
#endif

void Util::setDataPath( const string & str ){
    dataPath = str; 
}

Filesystem::AbsolutePath Util::getDataPath2(){
    return Filesystem::AbsolutePath(dataPath + "/");
}

/* FIXME: remove this method */
bool Util::exists( const string & file ){
#ifndef CROSS_BUILD
    return Storage::instance().exists(Filesystem::AbsolutePath(file));
#elif defined(FS_WRAPPER)
    return fs::exists(fs::path(Filesystem::AbsolutePath(file).path()));
#else
    return false;
#endif

/*
#ifdef USE_ALLEGRO
    return ::exists(file.c_str()) != 0;
#else
    return file_exists(file.c_str());
#endif
*/
}

/*
vector<Filesystem::AbsolutePath> Util::getFiles(const Filesystem::AbsolutePath & dataPath, const string & find){
    return Filesystem::getFiles(dataPath, find);
}
*/

string Util::trim(const std::string & str){
    string s;
    size_t startpos = str.find_first_not_of(" \t");
    size_t endpos = str.find_last_not_of(" \t");
    // if all spaces or empty return an empty string  
    if ((string::npos == startpos) ||
        (string::npos == endpos)){
        return "";
    } else {
        return str.substr(startpos, endpos-startpos+1);
    }
    return str;
}

/* makes the first letter of a string upper case */
string Util::upcase(std::string str){
    if ( str.length() > 0 && (str[0] >= 'a' && str[0] <= 'z') ){
        str[0] = str[0] - 'a' + 'A';
    }
    return str;
}

static int lowerCase(int c){
    return tolower(c);
}

static int upperCase(int c){
    return toupper(c);
}

string Util::upperCaseAll(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), upperCase);
    return str;
}

string Util::lowerCaseAll(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), lowerCase);
    /*
    for (unsigned int i = 0; i < str.length(); i++){
        if (str[0] >= 'A' && str[0] <= 'Z'){
            str[0] = str[0] - 'A' + 'a';
        }
    }
    */
    return str;
}

// Split string
std::vector<std::string> Util::splitString(std::string str, char splitter){
    std::vector<std::string> strings;
    size_t next = str.find(splitter);
    while (next != std::string::npos){
        strings.push_back(str.substr(0, next));
        str = str.substr(next+1);
        next = str.find(splitter);
    }
    if (str != ""){
        strings.push_back(str);
    }

    return strings;
}

// Join strings
std::string Util::joinStrings(const std::vector< std::string > & message, unsigned int start){
    std::string all;
    for (unsigned int i = start; i < message.size(); ++i){
        all+=message.at(i) + (i < message.size()-1 ? " " : "");
    }
    return all;
}

/*Gets the minimum of three values*/
static int minimum(int a,int b,int c){
  int min=a;
  if(b<min)
    min=b;
  if(c<min)
    min=c;
  return min;
}

/* Compute levenshtein distance between s and t
 * from: http://www.merriampark.com/ldc.htm
 */
static int levenshtein_distance(const char *s, const char *t){
  //Step 1
  int k,i,j,n,m,cost,distance;
  int * d;
  n=strlen(s); 
  m=strlen(t);
  if(n!=0&&m!=0){
    d = (int*) malloc((sizeof(int))*(m+1)*(n+1));
    m++;
    n++;
    //Step 2    
    for(k=0;k<n;k++)
    d[k]=k;
    for(k=0;k<m;k++)
      d[k*n]=k;
    //Step 3 and 4  
    for(i=1;i<n;i++)
      for(j=1;j<m;j++)
    {
        //Step 5
        if(s[i-1]==t[j-1])
          cost=0;
        else
          cost=1;
        //Step 6             
        d[j*n+i]=minimum(d[(j-1)*n+i]+1,d[j*n+i-1]+1,d[(j-1)*n+i-1]+cost);
      }
    distance=d[n*m-1];
    free(d);
    return distance;
  } else {
    return -1; //a negative return value means that one or both strings are empty.
  }
}

int Util::levenshtein(const std::string & str1, const std::string & str2){
    return levenshtein_distance(str1.c_str(), str2.c_str());
}

void Util::limitPrintf(char * buffer, int size, const char * format, va_list args){
#ifdef USE_ALLEGRO
    uvszprintf(buffer, size, format, args);
#else
    vsnprintf(buffer, size, format, args);
#endif
}

void Util::showError(const Graphics::Bitmap & screen, const Exception::Base & exception, const string & info){
    // screen.BlitFromScreen(0, 0);
    Graphics::Bitmap error(screen.getWidth() - 100, screen.getHeight() - 100);
    error.fill(Graphics::darken(Graphics::makeColor(160, 0, 0), 3));
    error.border(1, 2, Graphics::makeColor(240, 0, 0));
    const Font & font = Font::getDefaultFont(17, 17);
    int y = 10;
    std::ostringstream out;
    out << info;
    out << " " << exception.getTrace();
    Global::debug(0) << out.str() << std::endl;
    font.printfWrap(10, 10, Graphics::makeColor(240, 240, 240), error, error.getWidth() - 20, out.str(), 0);

    // Graphics::Bitmap::transBlender(0, 0, 0, 220);
    error.translucent(220).draw(50, 50, screen);
    screen.BlitToScreen();
}

void Util::showError(const Exception::Base & exception, const std::string & info){
    Graphics::Bitmap screen(*Graphics::screenParameter.current());
    showError(screen, exception, info);
}

static double twoDigits(double x){
    return (int) (x * 100) / 100.0;
}

string Util::niceSize(unsigned long size){
    const char sizes[] = {'b', 'k', 'm', 'g', 't'};
    double real = size;
    for (unsigned int i = 0; i < sizeof(sizes) / sizeof(const char); i++){
        if (real > 1024){
            real /= 1024.0;
        } else {
            ostringstream in;
            in << twoDigits(real) << sizes[i];
            return in.str();
        }
    }
    ostringstream in;
    in << twoDigits(real) << "t";
    return in.str();
}

std::string Util::join(const std::vector<std::string> & list, const std::string & middle){
    ostringstream out;

    bool first = true;
    for (std::vector<std::string>::const_iterator it = list.begin(); it != list.end(); it++){
        if (!first){
            out << middle;
        }

        out << *it;
        first = false;
    }

    return out.str();
}

double Util::distance(double x1, double y1, double x2, double y2){
    double xs = x1 - x2;
    double ys = y1 - y2;
    return sqrt(xs * xs + ys * ys);
}
