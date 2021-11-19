#include "libabyss/mpqstream.h"

LibAbyss::MPQStream::MPQStream(HANDLE mpq, std::string_view fileName) {
    if (!SFileOpenFileEx(mpq, (const char *) fileName.data(), SFILE_OPEN_FROM_MPQ, &_mpqFile)) {
        throw std::runtime_error("Failed to open file from MPQ");
    }
}

LibAbyss::MPQStream::~MPQStream() {
    SFileCloseFile(_mpqFile);
}

int LibAbyss::MPQStream::underflow() {
    if (gptr() == egptr()) {
        DWORD amountRead;
        if (!SFileReadFile(_mpqFile, _buffer, sizeof(_buffer), &amountRead, nullptr)) {
            if (GetLastError() != ERROR_HANDLE_EOF) {
                throw std::runtime_error("Error reading file from MPQ");
            }
        }
        setg(_buffer, _buffer, _buffer + amountRead);
    }

    return gptr() == egptr() ? traits_type::eof() : traits_type::to_int_type(*gptr());
}
