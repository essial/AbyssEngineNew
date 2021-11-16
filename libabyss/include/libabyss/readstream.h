#ifndef ABYSS_READSTREAM_H
#define ABYSS_READSTREAM_H

#include <span>
#include <cstdint>

namespace LibAbyss {

    class ReadStream {
    public:
        virtual void ReadBytes(std::span<uint8_t> buffer) = 0;
        virtual bool Eof() = 0;
        uint8_t ReadUint8();
        int8_t ReadInt8();
        uint16_t ReadUint16();
        int16_t ReadInt16();
        uint32_t ReadUint32();
        int32_t ReadInt32();
        uint64_t ReadUint64();
        int64_t ReadInt64();

        virtual ~ReadStream() = default;
    };

}

#endif //ABYSS_READSTREAM_H
