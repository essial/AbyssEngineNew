#ifndef ABYSS_RINGBUFFER_H
#define ABYSS_RINGBUFFER_H

#include <cinttypes>
#include <vector>
#include <span>

namespace AbyssEngine {
class RingBuffer {
  public:
    RingBuffer(uint32_t bufferSize);
    void PushData(std::span<const char> data);
    void ReadData(char *outBuffer, uint32_t size);

  private:
    const uint32_t _bufferSize;

    uint32_t _writePosition;
    uint32_t _readPosition;
    uint32_t _remaining;

    std::vector<char> _buffer;
};
}

#endif // ABYSS_RINGBUFFER_H
