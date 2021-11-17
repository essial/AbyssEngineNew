#ifndef ABYSS_MPQSTREAM_H
#define ABYSS_MPQSTREAM_H

#include <streambuf>
#include <vector>
#include "mpq.h"

namespace LibAbyss {

    class MPQStream : public std::basic_streambuf<char> {
    public:
        MPQStream(HANDLE mpq, std::string_view fileName);
        ~MPQStream();


    protected:
        int underflow() override;

        int uflow() override;

        int pbackfail(int_type c) override;

        std::streamsize showmanyc() override;

    private:
        HANDLE _mpq;
        HANDLE _mpqFile;
        const char *_begin;
        const char *_end;
        const char *_current;
        const char *_streamCurrent;
        std::vector<char> _fileData;

        void BufferData();
    };

}


#endif //ABYSS_MPQSTREAM_H
