#include <gtest/gtest.h>
#include <libabyss/bitstream.h>
#include <libabyss/readstream.h>

class TestStreamSource : public LibAbyss::ReadStream {
public:
    explicit TestStreamSource(std::span<uint8_t> source) : _pos(0), _buffer(source) {
    }

    void ReadBytes(std::span<uint8_t> buffer) override {
        for (unsigned char &i: buffer) {
            i = _buffer[_pos++];
        }
    }

    bool Eof() override {
        return _pos >= _buffer.size();
    }

private:
    std::span<uint8_t> _buffer;
    int _pos;
};


TEST(BitStream, ReadBits) {
    uint8_t data[] = {0x21, 0x43, 0x65, 0x87};
    auto source = TestStreamSource(data);
    auto bitStream = LibAbyss::BitStream(source);
    for (int i = 1; i <= 8; i++) {
        ASSERT_EQ(bitStream.ReadBits(4), i);
    }
}


TEST(BitStream, PeakByte) {
    uint8_t data[] = {0x21, 0x43};
    auto source = TestStreamSource(data);
    auto bitStream = LibAbyss::BitStream(source);
    bitStream.ReadBits(4);
    ASSERT_EQ(bitStream.PeekByte(), 0x32);
}

TEST(BitStream, IsEOF) {
    uint8_t data[] = {0x21};
    auto source = TestStreamSource(data);
    auto bitStream = LibAbyss::BitStream(source);
    bitStream.ReadBits(8);
    ASSERT_THROW(bitStream.ReadBits(1), std::range_error);
}
