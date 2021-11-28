#include "ringbuffer.h"
AbyssEngine::RingBuffer::RingBuffer(uint32_t bufferSize) : _bufferSize(bufferSize), _buffer(), _readPosition(0), _writePosition(0), _remaining(0) {
    _buffer.resize(bufferSize);
}
void AbyssEngine::RingBuffer::PushData(std::span<const char> data) {
    // Determine the bytes we have left to write before we hit the read position
    const auto remainingSize = _bufferSize - _remaining;

    // Grab the amount of data we are going to write
    const auto toWrite = data.size();

    // Determine the amount of overflow (if we are writing past the current read position)
    const auto overflow = (toWrite > remainingSize) ? (toWrite - remainingSize) : 0;

    // Start with the current write position
    auto writePos = _writePosition;

    // Wrap write position
    while (writePos >= _bufferSize)
        writePos -= _bufferSize;

    // Write all the bytes
    for (auto i = 0; i < toWrite; i++) {
        _buffer[writePos++] = data[i];

        // Wrap the writer index around to the beginning if we reached the end
        if (writePos >= _bufferSize)
            writePos -= _bufferSize;
    }

    // Add data to the data we can now read
    _remaining += toWrite - overflow;

    // Update write position
    _writePosition = writePos;

    // Bump the read position ahead if we have written past the current read position
    _readPosition += overflow;

    // Wrap the read position
    while (_readPosition >= _bufferSize)
        _readPosition -= _bufferSize;
}
void AbyssEngine::RingBuffer::ReadData(char *outBuffer, uint32_t size) {
    if (size == 0 || _remaining == 0) {
        memset(outBuffer, 0, size);
        return;
    }

    auto sizeLeft = size;
    auto readPos = _readPosition;

    while (readPos >= _bufferSize)
        readPos -= _bufferSize;

    for (auto i = 0; i < size; i++) {
        outBuffer[i] = _buffer[readPos++];
        sizeLeft--;

        if (--_remaining == 0) {
            // TODO: Maybe warn of a buffer under-run?
            memset(outBuffer+sizeLeft, 0, sizeLeft);
            return;
        }

        if (readPos >= _bufferSize)
            readPos -= _bufferSize;
    }
}
