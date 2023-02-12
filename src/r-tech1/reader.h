#ifndef _rtech1_reader_h
#define _rtech1_reader_h

namespace Util{

/* reads data from some place: files from the filesystem, files with a zipfile,
 * files over the network maybe.
 */
class Reader{
public:
    Reader();
};

class FileReader: public Reader {
public:
    FileReader();
};

class ZipReader: public Reader {
public:
    ZipReader();
};

}

#endif
