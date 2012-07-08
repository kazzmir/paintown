#include "util/bitmap.h"

#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <stdint.h>

#include "font.h"
#include "item.h"
#include "item-content.h"
#include "section.h"
#include "reader.h"
#include "util.h"
#include "globals.h"
#include "util/debug.h"
#include "util/funcs.h"
#include "util/load_exception.h"

using namespace std;

// If you use this, please delete the item after you use it, this isn't java ok
static MugenItemContent *getOpts(const std::string &opt){
    std::string contentHolder = "";
    MugenItemContent *temp = new MugenItemContent();
    const char * ignored = " \r\n";
    Global::debug(3) << "Parsing string to ItemContent: " << opt << endl;
    for (unsigned int i = 0; i < opt.size(); ++i ){
        if (opt[i] == ';'){
            break;
        }

        if (opt[i] == ' '){
            if( !contentHolder.empty() ) *temp << contentHolder;
            Global::debug(3) << "Got content: " << contentHolder << endl;
            contentHolder = "";
        }
        //Start grabbing our item
        else if (! strchr(ignored, opt[i])){
            contentHolder += opt[i];
        }
    }
    if( !contentHolder.empty() ){
        *temp << contentHolder;
        Global::debug(3) << "Got content: " << contentHolder << endl;
    }
    return temp;
}

namespace Mugen{

Font::Font(const Filesystem::AbsolutePath & file):
type(Fixed),
width(0),
height(0),
spacingx(0),
spacingy(0),
colors(0),
offsetx(0),
offsety(0),
pcx(NULL),
pcxsize(0){
    if (Mugen::Util::fixCase(file.getExtension()) != "fnt"){
        throw LoadException(__FILE__, __LINE__, "Font files must end with an .fnt extension");
    }
    std::string temp = file.path();
    temp = Mugen::Util::invertSlashes(temp);
    Global::debug(1) << "[mugen font] Opening file '" << temp << "'" << endl;
    myfile = temp;
    
    load(Filesystem::AbsolutePath(temp));
}

#if 0
Font::Font( const char * file ):
type(Fixed),
width(0),
height(0),
spacingx(0),
spacingy(0),
colors(0),
offsetx(0),
offsety(0),
pcx(0),
pcxsize(0),
currentBank(0){
    std::string temp = file;
    temp = Mugen::Util::invertSlashes(temp);
    Global::debug(1) << "[mugen font] Opening file '" << temp << "'" << endl;
    ifile.open( temp.c_str() );
    if (!ifile){
        perror("cant open file");
    }
    myfile = temp;
    
    load();
}
#endif

/* is this constructor ever used? should it make deep copies of bmp and pcx? */
Font::Font( const Font &copy ){
    this->type = copy.type;
    this->width = copy.width;
    this->height = copy.height;
    this->spacingx = copy.spacingx;
    this->spacingy = copy.spacingy;
    this->colors = copy.colors;
    this->offsetx = copy.offsetx;
    this->offsety = copy.offsety;
    this->pcx = copy.pcx;
    this->banks = copy.banks;
}

Font::~Font(){
    if (pcx){
        delete[] pcx;
    }
}
    
Font & Font::operator=( const Font &copy ){
    this->type = copy.type;
    this->width = copy.width;
    this->height = copy.height;
    this->spacingx = copy.spacingx;
    this->spacingy = copy.spacingy;
    this->colors = copy.colors;
    this->offsetx = copy.offsetx;
    this->offsety = copy.offsety;
    this->banks = copy.banks;
    
    return *this;
}

// Implement Font stuff
void Font::setSize( const int x, const int y ){
    // We don't change sizes
    if  ( (x < 0 || y < 0) && type!=Fixed ){
        return;
    }
}
int Font::getSizeX() const{
    return width;
}
int Font::getSizeY() const{
    return height;
}

int Font::textLength( const char * text ) const{
    std::string str(text);
    int size =0;
    for (unsigned int i = 0; i < str.size(); ++i){
        std::map<char, FontLocation>::const_iterator loc = positions.find(str[i]);
        if (loc != positions.end()){
            size += loc->second.width + spacingx;
        } else {
            // Couldn't find a position for this character assume regular width and skip to the next character
            size += width + spacingx;
        }
    }
    return size;
}

int Font::getHeight( const string & str ) const {
    // What? I guess this is for freetype?
    return getHeight();
}

int Font::getHeight() const {
    return height;
}
    
#if 0
void Font::printf( int x, int y, int xSize, int ySize, Graphics::Color color, const Graphics::Bitmap & work, const string & str, int marker, ... ) const {
    /* call the other printf somehow.. */
}
#endif

void Font::printf(int x, int y, int bank, const Graphics::Bitmap & work, const string & str, int marker, ... ){
    // Va list
    char buf[512];
    va_list ap;

    va_start(ap, marker);
    vsnprintf(buf, sizeof(buf), str.c_str(), ap);
    va_end(ap);

    const std::string newstr(buf);
            
    const PaintownUtil::ReferenceCount<Graphics::Bitmap> & font = changeBank(bank);
    if (font == NULL){
        return;
    }

    int workoffsetx = 0;
    for (unsigned int i = 0; i < newstr.size(); ++i){
        std::map<char, FontLocation>::const_iterator loc = positions.find(newstr[i]);
        if (loc != positions.end()){
            /*
            Bitmap character = Bitmap::temporaryBitmap(loc->second.width + spacingx, height + spacingy);
            character.clearToMask();
            bmp->Blit(loc->second.startx, 0, loc->second.width + spacingx, height + spacingy,0,0, character);
            */
            Graphics::Bitmap character(*font, loc->second.startx, 0, loc->second.width, height);
            character.draw(x + workoffsetx + offsetx, y + offsety, work);
            workoffsetx += loc->second.width + spacingx;
        } else{
            // Couldn't find a position for this character draw nothing, assume width, and skip to the next character
            workoffsetx += width + spacingx;
        }
    }
}

void Font::render(int x, int y, int position, int bank, const Graphics::Bitmap & work, const string & str){
    const int height = getHeight();
    const int length = textLength(str.c_str());
    switch (position){
	case -1:
	    printf(x - length, y - height, bank, work, str, 0);
	    break;
	case 1:
	    printf(x, y - height, bank, work, str, 0);
	    break;
	case 0:
	default:
	    printf(x - (length/2), y - height, bank, work, str, 0);
	    break;
    }
}

/* get a pointer to a specific bank. bank numbers start from 0 */
unsigned char * Font::findBankPalette(int bank) const {
    return pcx + pcxsize - ((bank+1) * colors * 3);
}

Graphics::Bitmap * Font::makeBank(int bank) const {
    unsigned char newpal[768];
    // Reset palette
    memcpy(pcx + (pcxsize - 768), palette, 768);
    memcpy(newpal, pcx+(pcxsize)-768, 768);
    unsigned char * newBank = findBankPalette(bank);
    // Global::debug(1) << "palette start: " << (void*)(pcx + pcxsize - 768) << ". new bank: " << (void*)newBank << endl;
    memcpy((void*) &newpal[768 - colors*3], (void*) newBank, colors * 3);

    /* hack to get around the 16-bit masking color */
    for (int i = 768 - colors * 3; i < 768; i += 3){
        int r = newpal[i];
        int g = newpal[i+1];
        int b = newpal[i+2];
        Graphics::Color col = Graphics::makeColor(r,g,b);
        if (col == Graphics::MaskColor()){
            Graphics::Color oldCol = col;
            while (oldCol == col){
                r -= 1;
                oldCol = Graphics::makeColor(r,g,b);
            }
            newpal[i] = r;
            newpal[i+1] = g;
            newpal[i+2] = b;
        }
    }

    memcpy(pcx + (pcxsize - 768), newpal, 768);
    Graphics::Bitmap * bmp = new Graphics::Bitmap(Graphics::memoryPCX((unsigned char*) pcx, pcxsize));
    bmp->replaceColor(bmp->get8BitMaskColor(), Graphics::MaskColor());
    return bmp;
}

PaintownUtil::ReferenceCount<Graphics::Bitmap> Font::changeBank(int bank){
    if (bank < 0 || bank > (colors -1)){
        return PaintownUtil::ReferenceCount<Graphics::Bitmap>(NULL);
    }

    if (banks[bank] == NULL){
        banks[bank] = makeBank(bank);
    }

    return banks[bank];
}

static void getline(PaintownUtil::ReferenceCount<Storage::File> file, string & out){
    ostringstream more;

    while (true){
        if (file->eof()){
            break;
        }
        char next;
        file->readLine(&next, 1);
        if (next == '\n'){
            break;
        }
        more << next;
    }

    out = more.str();
}

void Font::load(const Filesystem::AbsolutePath & path){
    PaintownUtil::ReferenceCount<Storage::File> ifile = Storage::instance().open(path);
    if (ifile == NULL){
        std::ostringstream out;
        out << "Can't find font file " << path.path();
        throw LoadException(__FILE__, __LINE__, out.str());
    }

    /* 16 skips the header stuff */
    /*
    int location = 16;
    // Lets go ahead and skip the crap -> (Elecbyte signature and version) start at the 16th byte
    ifile.seekg(location,ios::beg);
    */

    Storage::LittleEndianReader byteReader(ifile);
    uint8_t signature[12];
    byteReader.readBytes(signature, 12);
    uint16_t versionHigh = byteReader.readByte2();
    uint16_t versionLow = byteReader.readByte2();

    // Global::debug(0) << "Version for font " << versionHigh << " " << versionLow << std::endl;

    uint32_t pcxlocation = byteReader.readByte4();
    pcxsize = byteReader.readByte4();
    uint32_t txtlocation = byteReader.readByte4();
    uint32_t txtsize = byteReader.readByte4();
    Global::debug(3) << "PCX Location: " << pcxlocation << " | PCX Size: " << pcxsize << endl;
    Global::debug(3) << "TXT Location: " << txtlocation << " | TXT Actual location: " << pcxlocation + pcxsize << " | TXT Size: " << txtsize << endl;

    // Get the pcx load our bitmap
    ifile->seek(pcxlocation, ios::beg);
    pcx = new unsigned char[pcxsize];
    ifile->readLine((char *)pcx, pcxsize);
    memcpy(palette, pcx+(pcxsize)-768, 768);

    /*
    bmp = new Graphics::Bitmap(Graphics::Bitmap::memoryPCX((unsigned char*) pcx, pcxsize));
    bmp->replaceColor(bmp->get8BitMaskColor(), Graphics::MaskColor());
    */

    // Get the text
    ifile->seek(pcxlocation+pcxsize, ios::beg);
    std::vector<std::string> ourText;
    while (!ifile->eof()){
        std::string line;
        getline(ifile, line);
        ourText.push_back(line);
    }

    /* FIXME!! Replace with peg parser */
    MugenReader reader(ourText);
    
    std::vector< MugenSection * > collection = reader.getCollection();

    for (unsigned int i = 0; i < collection.size(); ++i){
        const std::string &head = collection[i]->getHeader();
        if (head == "Def"){
            while( collection[i]->hasItems() ){
                MugenItemContent *content = collection[i]->getNext();
                const MugenItem *item = content->getNext();
                std::string itemhead = item->query();
                itemhead = Mugen::Util::removeSpaces(itemhead);
                // This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
                itemhead = Mugen::Util::fixCase( itemhead );
                if ( itemhead.find("size")!=std::string::npos ){
                    *content->getNext() >> width;
                    *content->getNext() >> height;
                } else if ( itemhead.find("spacing")!=std::string::npos ){
                    *content->getNext() >> spacingx;
                    *content->getNext() >> spacingy;
                } else if ( itemhead.find("colors")!=std::string::npos ){
                    *content->getNext() >> colors;
                } else if ( itemhead.find("offset")!=std::string::npos ){
                    *content->getNext() >> offsetx;
                    *content->getNext() >> offsety;
                } else if ( itemhead.find("type")!=std::string::npos ){
                    std::string temp;
                    *content->getNext() >> temp;
                    temp = Mugen::Util::removeSpaces(temp);
                    temp = Mugen::Util::fixCase(temp);
                    if (temp == "variable")type = Variable;
                    else if (temp == "fixed")type = Fixed;
                    Global::debug(3) << "Type: " << temp << endl;
                } //else throw MugenException( "Unhandled option in Info Section: " + itemhead );
            }
            Global::debug(3) << "Size X: " << width << ", Size Y: " << height << ", Spacing X: " << spacingx << ", Spacing Y: " << spacingy << ", Colors: " << colors << ", Offset X: " << offsetx << ", Offset Y: " << offsety << endl;
        } else if (head == "Map"){
            bool beginParse = false;
            int locationx = 0;
            for (std::vector<std::string>::iterator l = ourText.begin(); l != ourText.end(); ++l){
                std::string line = *l;
                if (!beginParse){
                    if (line.find("[Map]")==std::string::npos){
                        continue;
                    } else{
                        beginParse = true;
                        continue;
                    }
                }
                MugenItemContent *opt = getOpts(line);
                std::string character;
                int startx = locationx * width;
                int chrwidth = width;
		if (opt->hasItems()){
		    *opt->getNext() >> character;
		    if (character.size() > 1){
			character = Mugen::Util::fixCase(character);
			// 0x5b
			if (character.compare("0x5b")==0)character = "[";
			// 0x3b
			else if (character.compare("0x3b")==0)character = ";";
		    }
                    if (type != Fixed){
                        // get other two
                        *opt->getNext() >> startx;
                        *opt->getNext() >> chrwidth;
                    }
                    FontLocation loc;
                    loc.startx = startx;
                    loc.width = chrwidth;
                    char code = character[0];
                    Global::debug(3) << "Storing Character: " << code << " | startx: " << loc.startx << " | width: " << loc.width << endl;
                    positions[code] = loc;
                }
                delete opt;
                ++locationx;
            }
        }
    }
}

}
