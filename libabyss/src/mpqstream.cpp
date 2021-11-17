#include "libabyss/mpqstream.h"

const int MPQ_STREAM_READ_MAX = 2048;

LibAbyss::MPQStream::MPQStream(HANDLE mpq, std::string_view fileName) : _mpq(mpq), _fileData() {
    if (!SFileOpenFileEx(_mpq, (const char *) fileName.data(), SFILE_OPEN_FROM_MPQ, &_mpqFile)) {
        throw std::runtime_error("Failed to open file from MPQ");
    }

    DWORD fileSize;
    SFileGetFileInfo(_mpqFile, SFileInfoFileSize, &fileSize, sizeof(DWORD), nullptr);

    _fileData.resize(fileSize);
    _begin = _fileData.data();
    _end = _begin + fileSize - 1;
    _current = _begin;
    _streamCurrent = _begin;
}

LibAbyss::MPQStream::~MPQStream() {
    SFileCloseFile(_mpqFile);
}


int LibAbyss::MPQStream::underflow() {
    BufferData();

    if (_current == _end)
        return traits_type::eof();

    return traits_type::to_int_type(*_current);
}

int LibAbyss::MPQStream::uflow() {
    BufferData();

    return (_current >= _end)
           ? traits_type::eof()
           : traits_type::to_int_type(*_current++);
}

int LibAbyss::MPQStream::pbackfail(int c) {
    BufferData();

    if (_current == _begin || (c != traits_type::eof() && c != _current[-1])) {
        return traits_type::eof();
    }
    return traits_type::to_int_type(*--_current);
}

std::streamsize LibAbyss::MPQStream::showmanyc() {
    return (_end - _current) - 1;
}

void LibAbyss::MPQStream::BufferData() {
    if (_current == _begin) {
        SFileSetFilePointer(_mpqFile, 0, nullptr, FILE_BEGIN);
    }

    if (_current < _streamCurrent) {
        return;
    }

    auto streamPos = (_streamCurrent - _begin);
    auto remaining = (int) (_end - _current);
    auto toRead = remaining < MPQ_STREAM_READ_MAX ? remaining : MPQ_STREAM_READ_MAX;
    DWORD amountRead;
    if (!SFileReadFile(_mpqFile, _fileData.data() + streamPos, toRead, &amountRead, nullptr)) {
        if (GetLastError() == ERROR_HANDLE_EOF) {
            return;
        }

        throw std::runtime_error("Error reading file from MPQ");
    }

    _streamCurrent += amountRead;
}
