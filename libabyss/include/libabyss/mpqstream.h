#ifndef ABYSS_MPQSTREAM_H
#define ABYSS_MPQSTREAM_H

#include <streambuf>
#include <vector>
#include "mpq.h"
#include <libabyss/mpq.h>

namespace LibAbyss {

    class MPQStream : public std::basic_streambuf<char> {
    public:
        MPQStream(HANDLE mpq, std::string_view fileName);

        ~MPQStream() override;

    protected:
        int underflow() override;

    private:
        HANDLE _mpqFile;
        char _buffer[2048];
    };

}

#endif //ABYSS_MPQSTREAM_H
