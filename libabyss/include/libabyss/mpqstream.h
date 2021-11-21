#ifndef ABYSS_MPQSTREAM_H
#define ABYSS_MPQSTREAM_H

#include <ios>
#include <streambuf>
#include <vector>
#include "mpq.h"
#include <libabyss/mpq.h>

namespace LibAbyss {

    class MPQStream : public std::basic_streambuf<char> {
    public:
        MPQStream(HANDLE mpq, const std::string &fileName);

        ~MPQStream() override;

        std::streamsize StartOfBlockForTesting() const {
            return _startOfBlock;
        }

    protected:
        int underflow() override;
        pos_type seekpos(pos_type pos,
                         std::ios_base::openmode which) override;
        pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                         std::ios_base::openmode which) override;

    private:
        HANDLE _mpqFile;
        std::streamsize _startOfBlock = 0;
        char _buffer[2048];
    };

}

#endif //ABYSS_MPQSTREAM_H
