//! Constructor
        freetype::freetype(const Filesystem::AbsolutePath & str, const int x, const int y ):
            face(NULL){
                //Load library
                if (!ftLibrary){
                    if (FT_Init_FreeType(&ftLibrary) != 0){
                        throw Exception("Could not initialize freetype");
                    }
                }
                instances += 1;
                faceLoaded = kerning = false;
                currentIndex = 0;
                currentFilename = "";
                faceName = "";
                // currentChar = new character;
                systemName = "";
                internalFix = false;

                this->load(str, 0, x, y );
            }

    //! Destructor
        freetype::~freetype(){

            //if(face!=NULL)FT_Done_Face(face);
            if (faceLoaded && face != NULL){
                FT_Done_Face(face);
                face = NULL;
            }

            instances -= 1;
            if (instances == 0){
                FT_Done_FreeType(ftLibrary);
                ftLibrary = NULL;
            }

            destroyGlyphIndex();

            /*
            if ( currentChar ){
                delete currentChar;
            }
            */
        }

        void freetype::destroyGlyphIndex(){
            for (std::map<int, std::map<signed long, character*> >::iterator i1 = fontTable.begin(); i1 != fontTable.end(); i1++){
                std::map<signed long, character*> & characters = (*i1).second;
                for (std::map<signed long, character*>::iterator i2 = characters.begin(); i2 != characters.end(); i2++){
                    character * character = (*i2).second;
                    delete character;
                }
            }
        }

    // Extract glyph
        character * freetype::extractGlyph(signed long unicode){
            int w, h, ew;
            character * tempChar = new character();

            // Translate it according to the given italics
            double italics = (double)(size.italics)*GLYPH_PI/180;

            FT_Matrix matrix;
            matrix.xx = 0x10000L;
            matrix.xy = (FT_Fixed)( sin( italics ) * (GLYPH_SQRT2*0x10000L) );
            matrix.yx = 0;
            matrix.yy = 0x10000L;
            FT_Set_Transform( face, &matrix, 0 );

            if (FT_Load_Char(face, unicode, FT_LOAD_TARGET_NORMAL | FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT) != 0){
                throw Exception("Could not load freetype glyph");
            }

            w = face->glyph->bitmap.width;
            h = face->glyph->bitmap.rows;
            ew = 0;
            if (!w)ew = 1;
            if (!h)h = 1;

            tempChar->width = (w + ew);
            tempChar->height = h;

            tempChar->rows = face->glyph->bitmap.rows;
            tempChar->grays = face->glyph->bitmap.num_grays;
            tempChar->pitch = face->glyph->bitmap.pitch;
            tempChar->line = new unsigned char[tempChar->rows * tempChar->pitch];
            memcpy(tempChar->line, face->glyph->bitmap.buffer, tempChar->rows * tempChar->pitch);

            tempChar->left = face->glyph->bitmap_left;
            tempChar->top = face->glyph->bitmap_top;
            tempChar->right = face->glyph->advance.x >> 6;
            tempChar->unicode = unicode;

            tempChar->length = ((w + ew)+face->glyph->advance.x) >> 6;

            return tempChar;
        }

    // Create single index
        void freetype::createIndex(){
            std::map<int, std::map<signed long, character*> >::iterator p;
            p = fontTable.find(size.createKey());
            if (p == fontTable.end()){
                if (FT_Set_Pixel_Sizes(face, size.width, size.height) != 0){
                    throw Exception("Could not set freetype size");
                }
                FT_UInt glyphIndex;
                FT_ULong unicode = FT_Get_First_Char(face, &glyphIndex);
                std::map<signed long, character*> tempMap;
                while (glyphIndex != 0){
                    tempMap.insert(std::make_pair(unicode, extractGlyph(unicode)));
                    unicode = FT_Get_Next_Char(face, unicode, &glyphIndex);
                }

                fontTable.insert(std::make_pair(size.createKey(), tempMap));
            }

            if (fontTable.find(size.createKey()) == fontTable.end()){
                printf("ftalleg: inconsistency error\n");
                throw Exception("inconsistency error");
            }
        }

        /*
    pixeler getPutPixel(){
        switch( get_color_depth() ){
            case 8 : return _putpixel;
            case 15 : return _putpixel15;
            case 16 : return _putpixel16;
            case 24 : return _putpixel24;
            case 32 : return _putpixel32;
            default : return putpixel;
        }
    }
        */

        void drawOneCharacter(const character * tempChar, int & x1, int & y1, FT_UInt sizeHeight, const Graphics::Bitmap & bitmap, const Graphics::Color & color){
            unsigned char * line = tempChar->line;
            int colorRed = Graphics::getRed(color);
            int colorGreen = Graphics::getGreen(color);
            int colorBlue = Graphics::getBlue(color);

            /* cache the last color, there is a good chance it will be reused */
            unsigned char lastData = -1;
            short lastGrays = -1;
            Graphics::Color black = Graphics::makeColor(0, 0, 0);
            Graphics::Color lastColor = black;

            for (int y = 0; y < tempChar->rows; y++){
                unsigned char * buffer = line;
                for (int x = 0; x < tempChar->width; x++){
                    Graphics::Color finalColor = black;
                    unsigned char current = *buffer;
                    buffer++;
                    if (current == lastData && lastGrays == tempChar->grays){
                        finalColor = lastColor;
                    } else {
                        Graphics::Color col = fixColor(current, tempChar->grays);

                        int red = Graphics::getRed(col);
                        int green = Graphics::getGreen(col);
                        int blue = Graphics::getBlue(col);

                        if ((red < 50) ||
                            (green < 50) ||
                            (blue < 50)){
                            continue;
                        }

                        red = red * colorRed / 255;
                        green = green * colorGreen / 255;
                        blue = blue * colorBlue / 255;

                        finalColor = Graphics::makeColor(red, green, blue);
                        lastData = current;
                        lastColor = finalColor;
                        lastGrays = tempChar->grays;
                    }

                    //col.alpha= col.alpha * color.alpha / 255;
                    // putpixel(bitmap,x1+tempChar.left+x,y1 - tempChar.top+y + size.height,makecol(red,blue,green));
                    /* dangerous! putter is probably one of the _putpixel* routines so if x or y are off the bitmap
                     * you will get a segfault
                     */
                    // putter(bitmap,x1+tempChar.left+x,y1 - tempChar.top+y + size.height,makecol(red,blue,green));
                    // putter = 0;
                    int finalX = x1+tempChar->left+x;
                    int finalY = y1 - tempChar->top + y + sizeHeight;
                    bitmap.putPixelNormal(finalX, finalY, finalColor);
                }
                line += tempChar->pitch;
            }
            x1 += tempChar->right;
        }

    // Render a character from the lookup table
        void freetype::drawCharacter(signed long unicode, int &x1, int &y1, const Graphics::Bitmap & bitmap, const Graphics::Color &color){

            // pixeler putter = getPutPixel();

            std::map<int, std::map<signed long, character*> >::iterator ft;
            ft = fontTable.find(size.createKey());
            if (ft != fontTable.end()){
                std::map<signed long, character*>::iterator p;
                p = (ft->second).find(unicode);
                if (p != ft->second.end()){
                    const character * tempChar = p->second;
                    drawOneCharacter(tempChar, x1, y1, size.height, bitmap, color);
                }
            }
        }

    //! Load font from memory
        bool freetype::load(const unsigned char *memoryFont, unsigned int length, int index, unsigned int width, unsigned int height) {
            if(!FT_New_Memory_Face(ftLibrary,memoryFont, length,index,&face)) {
                currentFilename = "memoryFont";
                currentIndex = index;
                faceLoaded = true;
                size.italics = 0;
                setSize(width, height);
                if (FT_HAS_GLYPH_NAMES(face)){
                    char buff[1024];
                    if(!FT_Get_Glyph_Name(face, currentIndex,buff, sizeof(buff))){
                        faceName = currentFilename;
                    }
                    else faceName = std::string(buff);
                } else {
                    faceName = currentFilename;
                }
                if(FT_HAS_KERNING(face))kerning=true;
                else kerning = false;
            } else {
                faceLoaded=false;
                std::cout << "Load system font failed\n";
            }

            return faceLoaded;
        }

    //! Load font from file
        bool freetype::load(const Filesystem::AbsolutePath & filename, int index, unsigned int width, unsigned int height){
            FT_Error error = FT_New_Face(ftLibrary, filename.path().c_str(), index, &face);
            if (error == 0){
                currentFilename = filename.path();
                currentIndex = index;
                faceLoaded = true;
                size.italics = 0;
                setSize(width, height);
                if (FT_HAS_GLYPH_NAMES(face)){
                    char buff[1024];
                    if (!FT_Get_Glyph_Name(face, currentIndex, buff, sizeof(buff))) {
                        faceName = currentFilename;
                    } else {
                        faceName = std::string(buff);
                    }
                } else {
                    faceName = currentFilename;
                }

                kerning = FT_HAS_KERNING(face);
            } else {
                faceLoaded = false;
                std::ostringstream fail;
                fail << "Could not load freetype font " << filename.path() << " error code " << error;
                throw Exception(fail.str());
            }

            return faceLoaded;
        }

    //! Get text length
        int freetype::getLength(const std::string & text) {
            Util::Thread::ScopedLock locked(lock);
            int length=0;
            std::map<int, std::map<signed long, character*> >::iterator ft;
            ft = fontTable.find(size.createKey());
            if (ft != fontTable.end()){
                for (unsigned int i = 0; i < text.length(); i++) {
                    std::map<signed long, character*>::iterator p;
                    signed long unicode = Utf::readUtf8CodePoint(text, &i);
                    p = (ft->second).find(unicode);
                    if (p != (ft->second).end()){
                        if (p != fontTable[size.createKey()].end()){
                            length += (p->second)->length;
                        }
                    }
                }
            }
            return length;
        }

    //! Render font to a bitmap
        void freetype::render(int x, int y, const Graphics::Color & color, const Graphics::Bitmap & bmp, ftAlign alignment, const std::string & text, int marker ...) {
            if (faceLoaded){
                int rend_x = 0;
                int rend_y = 0;
                std::ostringstream str;

                /* use vsnprintf/Util::limitPrintf here? */

                // Get extra arguments
                va_list ap;
                va_start(ap, marker);
                for(unsigned int i = 0; i<text.length();++i) {
                    if (text[i] == '%') {
                        if(text[i+1]=='s') {
                            str << va_arg(ap, char *);
                            ++i;
                        } else if(text[i+1]=='d'||text[i+1]=='i') {
                            str << va_arg(ap, signed int);
                            ++i;
                        } else if(text[i+1]=='c') {
                            str << (char)va_arg(ap, int);
                            ++i;
                        } else str << text[i];
                    } else {
                        str << text[i];
                    }
                }
                va_end(ap);

                std::string fixedText(str.str());
                switch (alignment) {
                    case ftLeft:
                        rend_x = x;
                        rend_y = y;
                        break;
                    case ftCenter:
                        rend_x = x - getLength(fixedText)/2;
                        rend_y = y;
                        break;
                    case ftRight:
                        rend_x = x - getLength(fixedText);
                        rend_y = y;
                        break;
                    default:
                        rend_x = x;
                        rend_y = y;
                        break;
                }

                int previous = 0;
                int next = 0;
                for (unsigned int i = 0; i<fixedText.length(); i++){
                    long unicode = Utf::readUtf8CodePoint(fixedText, &i);
                    if (kerning && previous && next){
                        next = FT_Get_Char_Index(face, unicode);
                        FT_Vector delta;
                        FT_Get_Kerning(face, previous, next, FT_KERNING_DEFAULT, &delta);
                        rend_x += delta.x >> 6;
                        previous = next;
                    }
                    drawCharacter(unicode, rend_x, rend_y, bmp, color);
                }
            }
        }
            
        int freetype::calculateMaximumHeight(){
            Util::Thread::ScopedLock locked(lock);
            /* uhh, comment out the printf's ?? */
            std::map<int, std::map<signed long, character*> >::iterator ft;
            ft = fontTable.find(size.createKey());
            int top = 0;
            long code = 0;
            if ( ft != fontTable.end() ){
                std::map<signed long, character*>::iterator p;
                std::map< signed long, character* > & map = ft->second;
                for ( p = map.begin(); p != map.end(); p++ ){
                    const character * ch = p->second;
                    printf( "%c( %ld ). top = %d. rows = %d. total = %d\n", (char) ch->unicode, ch->unicode, ch->top, ch->rows, ch->top + ch->rows );
                    if ( ch->top + ch->rows > top ){
                        code = ch->unicode;
                        top = ch->top + ch->rows;
                    }
                }
            }
            printf( "Largest letter = %ld\n", code );
            return top;
        }

        int freetype::height(long code) const {
            Util::Thread::ScopedLock locked(lock);
            std::map<int, std::map<signed long, character*> >::const_iterator ft;
            ft = fontTable.find(size.createKey());
            if (ft != fontTable.end()){
                std::map<signed long, character*>::const_iterator p;
                p = (ft->second).find( code );
                if ( p != (ft->second).end() ){
                    const character * temp = p->second;
                    // printf( "%c top = %d rows = %d\n", (char) code, temp.top, temp.rows );
                    return temp->top + temp->rows;
                }
            } else {
                throw Exception("Internal inconsistency");
            }

            return 0;
        }

    int freetype::calculateHeight(const std::string & str) const {
            int max = 0;
            for ( unsigned int i = 0; i < str.length(); i++ ){
                int q = height(str[i]);
                // printf( "Height of %c is %d\n", str[ i ], q );
                if (q > max){
                    max = q;
                }
            }

            return max;
    }

    //! Set size
    void freetype::setSize( unsigned int w, unsigned int h){
            Util::Thread::ScopedLock locked(lock);
            if ( w != size.width || h != size.height ){
                if (internalFix)return;
                if (w<=0 || h<=0)return;
                size.width = w;
                size.height = h;
                createIndex();
                // maximumHeight = calculateMaximumHeight();
            }
    }

    //! Set italics
    void freetype::setItalics(int i){
            Util::Thread::ScopedLock locked(lock);
        if(internalFix)return;
        if(i<-45)i=(-45);
        else if(i>45)i=45;
        size.italics = i;
        createIndex();
    }

        void freetype::getSize(int * w, int * h) const {
            *w = size.width;
            *h = size.height;
        }

    //! Get Width
    int freetype::getWidth() const {
            return size.width;
    }

    //! Get Height
    int freetype::getHeight( const std::string & str ) const {
            // return size.height;
            return calculateHeight(str);
        }

    //! Get Italics
    int freetype::getItalics(){
            return size.italics;
    }

