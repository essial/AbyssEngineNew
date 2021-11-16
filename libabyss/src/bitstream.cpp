#include "../include/libabyss/bitstream.h"

LibAbyss::BitStream::BitStream(LibAbyss::ReadStream &stream)
        : _stream(stream), _current(0), _bitCount(0) {

}

uint32_t LibAbyss::BitStream::ReadBits(unsigned int numberOfBits) {
    EnsureBits(numberOfBits);

    auto result = _current & (0xFFFF >> (unsigned int)(16 - numberOfBits));
    WasteBits(numberOfBits);

    return result;
}

uint8_t LibAbyss::BitStream::PeekByte() {
    EnsureBits(8);

    return _current & 0xFF;
}

void LibAbyss::BitStream::WasteBits(unsigned int numberOfBits) {
    _current >>= numberOfBits;
    _bitCount -= numberOfBits;
}

void LibAbyss::BitStream::EnsureBits(unsigned int numberOfBits) {
    if (numberOfBits <= _bitCount)
        return;

    if (_stream.Eof())
        throw std::range_error("read past end of stream");

    _current |= (uint32_t)_stream.ReadUint8() << _bitCount;
    _bitCount += 8;
}
