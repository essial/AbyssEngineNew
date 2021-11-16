#include "../include/libabyss/readstream.h"

uint8_t LibAbyss::ReadStream::ReadUint8() {
    uint8_t result[1] = {};
    ReadBytes(result);
    return result[0];
}

int8_t LibAbyss::ReadStream::ReadInt8() {
    uint8_t result[1] = {};
    ReadBytes(result);
    return (int8_t) result[0];
}

uint16_t LibAbyss::ReadStream::ReadUint16() {
    uint8_t result[2] = {};
    ReadBytes(result);
    return (uint16_t) result[0]
           | ((uint16_t) result[1] << 8);
}

int16_t LibAbyss::ReadStream::ReadInt16() {
    return (int16_t) ReadUint16();
}

uint32_t LibAbyss::ReadStream::ReadUint32() {
    uint8_t result[4] = {};
    ReadBytes(result);
    return (uint32_t) result[0]
           | ((uint32_t) result[1] << 8)
           | ((uint32_t) result[2] << 16)
           | ((uint32_t) result[3] << 24);
}

int32_t LibAbyss::ReadStream::ReadInt32() {
    return (int32_t) ReadUint32();
}

uint64_t LibAbyss::ReadStream::ReadUint64() {
    uint8_t result[8] = {};
    ReadBytes(result);
    return (uint64_t) result[0]
           | ((uint64_t) result[1] << 8)
           | ((uint64_t) result[2] << 16)
           | ((uint64_t) result[3] << 24)
           | ((uint64_t) result[4] << 32)
           | ((uint64_t) result[5] << 40)
           | ((uint64_t) result[6] << 48)
           | ((uint64_t) result[7] << 56);
}

int64_t LibAbyss::ReadStream::ReadInt64() {
    return (int64_t)ReadUint64();
}
