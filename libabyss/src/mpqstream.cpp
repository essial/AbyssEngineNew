#include "libabyss/mpqstream.h"
#include <ios>

namespace LibAbyss {

MPQStream::MPQStream(HANDLE mpq, std::string_view fileName) {
    if (!SFileOpenFileEx(mpq, (const char *) fileName.data(), SFILE_OPEN_FROM_MPQ, &_mpqFile)) {
        throw std::runtime_error("Failed to open file from MPQ");
    }
}

MPQStream::~MPQStream() {
    SFileCloseFile(_mpqFile);
}

int MPQStream::underflow() {
    if (gptr() == egptr()) {
        _startOfBlock += egptr() - eback();
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

MPQStream::pos_type MPQStream::seekpos(pos_type pos,
        std::ios_base::openmode which) {
    return seekoff(pos, std::ios_base::beg, which);
}

MPQStream::pos_type MPQStream::seekoff(off_type off, std::ios_base::seekdir dir,
        std::ios_base::openmode) {
    switch (dir) {
        case std::ios_base::beg:
        case std::ios_base::cur:
            {
            std::streamsize newPos = off;
            if (dir == std::ios_base::cur) {
                newPos += _startOfBlock;
                newPos += gptr() - eback();
            }
            if (newPos >= _startOfBlock && newPos < _startOfBlock + (egptr() - eback())) {
                // The new position is already in the buffer, just repoint the pointer to it
                setg(eback(), eback() + newPos - _startOfBlock, egptr());
            } else {
                // Drop buffer, it will be read in underflow
                SFileSetFilePointer(_mpqFile, newPos, nullptr, 0);
                setg(nullptr, nullptr, nullptr);
                _startOfBlock = newPos;
            }
            break;
            }
        case std::ios_base::end:
            {
            DWORD fileSize = SFileSetFilePointer(_mpqFile, off, nullptr, 2);
            setg(nullptr, nullptr, nullptr);
            _startOfBlock = fileSize + off;
            break;
            }
        default:
            break;
    }
    return _startOfBlock + (gptr() - eback());
}

}
