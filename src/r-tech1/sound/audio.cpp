#include <string.h>

#include "r-tech1/sound/audio.h"
#include "r-tech1/debug.h"

namespace Util{

AudioConverter::AudioConverter(Encoding inputEncoding, int inputChannels, int inputFrequency,
               Encoding outputEncoding, int outputChannels, int outputFrequency):
buffer(NULL),
bufferSize(0){

    input.bytes = inputEncoding;
    input.channels = inputChannels;
    input.frequency = inputFrequency;

    output.bytes = outputEncoding;
    output.channels = outputChannels;
    output.frequency = outputFrequency;

    sizeRatio = (double) byteSize(output) * output.frequency / ((double) byteSize(input) * input.frequency);
}
    
int AudioConverter::byteSize(const Format & what){
    return encodingBytes(what.bytes) * what.channels;
}
    
/* how many bytes an encoding takes up */
int AudioConverter::encodingBytes(Encoding what){
    switch (what){
        case Unsigned8: return 1;
        case Signed16: return 2;
        case Unsigned16: return 2;
        case Float32: return 4;
    }

    return 1;
}
 
unsigned int AudioConverter::convertedLength(int length){
    // return length * sizeRatio;

    int total = length * sizeRatio;

    /* make sure we get an even number of samples */
    if (total % byteSize(output) != 0){
        total -= total % byteSize(output);
    }
    return total;
}

double CubicInterpolate(double y0, double y1,
                        double y2, double y3,
                        double mu){
    double a0,a1,a2,a3,mu2;

    mu2 = mu*mu;
    a0 = y3 - y2 - y0 + y1;
    a1 = y0 - y1 - a0;
    a2 = y2 - y0;
    a3 = y1;

    return (a0*mu*mu2+a1*mu2+a2*mu+a3);
}

template <class Input, class Output>
Output clamp(double input){
    Output top = (1 << (sizeof(Output) * 8 - 1)) - 1;
    Output bottom = -(1 << (sizeof(Output) * 8 - 1));
    if (input > top){
        return top;
    }
    if (input < bottom){
        return bottom;
    }
    return input;
}

template <>
signed short clamp<unsigned char>(double input){
    return ((input - 127) / 255) * (1 << (sizeof(signed short) * 8 - 1));
}

template <>
unsigned char clamp<unsigned char>(double input){
    if (input > 255){
        input = 255;
    }
    if (input < 0){
        input = 0;
    }
    return input;
}

template <>
float clamp<float>(double input){
    if (input > 1){
        input = 1;
    }
    if (input < -1){
        input = -1;
    }
    return input;
}


template <>
unsigned short clamp<signed short>(double input){
    return (int) clamp<signed short, signed short>(input) + (int) (1 << (sizeof(signed short) * 8 - 1));
}

template <>
float clamp<short unsigned int>(double input){
    double out = input / (1 << (sizeof(unsigned short) * 8));
    if (out > 1){
        return 1;
    }
    if (out < -1){
        return -1;
    }
    return out;
}

template <>
float clamp<unsigned char>(double input){
    double out = input / (1 << (sizeof(unsigned char) * 8));
    if (out > 1){
        return 1;
    }
    if (out < -1){
        return -1;
    }
    return out;

}

template <>
unsigned char clamp<signed short>(double input){
    double out = input / (1 << (sizeof(signed short) * 8 - 1));
    if (out > 1){
        return 1;
    }
    if (out < -1){
        return -1;
    }
    /* -1,1 -> 0,255 */
    return (unsigned char)((out + 1) * 128);
}

template <>
float clamp<signed short>(double input){
    double out = input / (1 << (sizeof(signed short) * 8 - 1));
    if (out > 1){
        return 1;
    }
    if (out < -1){
        return -1;
    }
    return out;
}

template <class SizeInput, class SizeOutput>
void doConvertRate(SizeInput * input, int inputSamples, int inputChannels, SizeOutput * buffer, int outputSamples, int outputChannels, double ratio){
    for (int sample = 0; sample < outputSamples; sample += 1){
        double inputSample = sample / ratio;

        for (int channel = 0; channel < outputChannels; channel += 1){

            int inputChannel = inputChannels > channel ? channel : inputChannels - 1;

            int sample0 = ((int) inputSample - 1) * inputChannels + inputChannel;
            int sample1 = ((int) inputSample + 0) * inputChannels + inputChannel;
            int sample2 = ((int) inputSample + 1) * inputChannels + inputChannel;
            int sample3 = ((int) inputSample + 2) * inputChannels + inputChannel;

            if (sample0 < 0){
                sample0 = sample1;
            }

            if (sample2 >= inputSamples * inputChannels){
                sample2 = sample1;
            }

            if (sample3 >= inputSamples * inputChannels){
                sample3 = sample2;
            }

            buffer[sample * outputChannels + channel] = clamp<SizeInput, SizeOutput>(CubicInterpolate(input[sample0], input[sample1], input[sample2], input[sample3], inputSample - (int) inputSample));

            // Global::debug(0) << "Input[" << sample << "] " << channel << ": " << input[sample1] << " Output: " << buffer[sample * 2 + channel] << std::endl;
        }
    }
}

template <class Input, class Output>
void doConvert3(void * input, int inputLength, int inputChannels,
                void * output, int outputLength, int outputChannels,
                double ratio){
    doConvertRate<Input, Output>((Input*) input, inputLength / sizeof(Input) / inputChannels, inputChannels,
                                 (Output*) output, outputLength / sizeof(Output) / outputChannels, outputChannels,
                                 ratio);
}

template <class Input>
void doConvert2(void * input, int inputLength, int inputChannels,
                Encoding outputType, void * output, int outputLength, int outputChannels,
                double ratio){
    switch (outputType){
        case Unsigned8: doConvert3<Input, unsigned char>(input, inputLength, inputChannels, output, outputLength, outputChannels, ratio); break;
        case Signed16: doConvert3<Input, signed short>(input, inputLength, inputChannels, output, outputLength, outputChannels, ratio); break;
        case Unsigned16: doConvert3<Input, unsigned short>(input, inputLength, inputChannels, output, outputLength, outputChannels, ratio); break;
        case Float32: doConvert3<Input, float>(input, inputLength, inputChannels, output, outputLength, outputChannels, ratio); break;
    }
}

void doConvert1(Encoding inputType, void * input, int inputLength, int inputChannels,
                Encoding outputType, void * output, int outputLength, int outputChannels, double ratio){
    switch (inputType){
        case Unsigned8: doConvert2<unsigned char>(input, inputLength, inputChannels, outputType, output, outputLength, outputChannels, ratio); break;
        case Signed16: doConvert2<signed short>(input, inputLength, inputChannels, outputType, output, outputLength, outputChannels, ratio); break;
        case Unsigned16: doConvert2<unsigned short>(input, inputLength, inputChannels, outputType, output, outputLength, outputChannels, ratio); break;
        case Float32: doConvert2<float>(input, inputLength, inputChannels, outputType, output, outputLength, outputChannels, ratio); break;
    }
}

int AudioConverter::convert(void * input, int length){
    /* no conversion needed */
    if (this->input == this->output){
        return length;
    }

    int total = convertedLength(length);
    /*
    if (total % byteSize(output) != 0){
        total -= total % byteSize(output);
    }
    */

    /* cache the buffer for future use */
    if (total > bufferSize){
        delete[] buffer;
        bufferSize = total;
        buffer = new char[bufferSize];
    }

    double frequencyRatio = (double) output.frequency / (double) this->input.frequency;

    doConvert1(this->input.bytes, input, length, this->input.channels, output.bytes, buffer, total, output.channels, frequencyRatio);

    memcpy(input, buffer, total);

    return total;
}

AudioConverter::~AudioConverter(){
    delete[] buffer;
}
        
bool AudioConverter::Format::operator==(const AudioConverter::Format & him) const {
    return this->bytes == him.bytes &&
           this->channels == him.channels &&
           this->frequency == him.frequency;
}

}
