#ifndef _rtech1_audio_h
#define _rtech1_audio_h

/* Deals with audio conversion between any source format and any destination format.
 * A format consists of
 *   byte encoding (8/16/32 bit, signed/unsigned, floating point/integer)
 *   number of channels (mono/stereo)
 *   frequency (22050hz, 44100hz, arbitrary hz)
 */

namespace Util{

/* endianness is always native */
enum Encoding{
    Unsigned8,
    Signed16,
    Unsigned16,
    Float32
};

class AudioConverter{
public:
    AudioConverter(Encoding inputEncoding, int inputChannels, int inputFrequency,
                   Encoding outputEncoding, int outputChannels, int outputFrequency);
    
    /* given some input length, return how long the converted output will be */
    unsigned int convertedLength(int length);

    /* convert the audio, put the output in the same buffer passed in -- 'input'
     * and returns the number of converted samples.
     * 'length' is the number of input samples in *bytes*
     * 'input' should be large enough to hold convertedLength(length) samples
     */
    int convert(void * input, int length);

    virtual ~AudioConverter();

protected:
    struct Format{
        Encoding bytes;
        int channels;
        int frequency;

        bool operator==(const Format & him) const;
    };
    
    int byteSize(const Format & what);
    int encodingBytes(Encoding what);

    Format input, output;
    double sizeRatio;
    char * buffer;
    int bufferSize;
};

}

#endif
