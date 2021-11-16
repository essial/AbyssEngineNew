#ifndef ABYSS_BITSTREAM_H
#define ABYSS_BITSTREAM_H


#include "readstream.h"

namespace LibAbyss {

    class BitStream {
    public:
        explicit BitStream(ReadStream &stream);

        uint32_t ReadBits(unsigned int numberOfBits);

        uint8_t PeekByte();

        void WasteBits(unsigned int numberOfBits);

    private:
        void EnsureBits(unsigned int numberOfBits);
        LibAbyss::ReadStream &_stream;
        uint32_t _current;
        uint8_t _bitCount;
    };

}

#endif //ABYSS_BITSTREAM_H
