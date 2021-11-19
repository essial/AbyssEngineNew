#ifndef ABYSS_INPUTSTREAM_H
#define ABYSS_INPUTSTREAM_H

#include <fstream>
#include <istream>
#include <streambuf>
#include <memory>

namespace LibAbyss {
    class InputStream : public std::istream {
    public:
        explicit InputStream(std::unique_ptr<std::streambuf> streamBuff) : std::istream(streamBuff.get()),
                                                                           _streamBuff(std::move(streamBuff)) {};

    private:
        std::unique_ptr<std::streambuf> _streamBuff;
        std::unique_ptr<std::ifstream> _ifstream;
    };
}

#endif // ABYSS_INPUTSTREAM_H
