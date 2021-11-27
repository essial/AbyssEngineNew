#ifndef LIBABYSS_INPUTSTREAM_H
#define LIBABYSS_INPUTSTREAM_H

#include <istream>
#include <streambuf>
#include <memory>

namespace LibAbyss {
    class InputStream : public std::istream {
    public:
        explicit InputStream(std::unique_ptr<std::streambuf> streamBuff) : std::istream(streamBuff.get()),
                                                                           _streamBuff(std::move(streamBuff)) {}

    private:
        [[maybe_unused]] std::unique_ptr<std::streambuf> _streamBuff;
    };
}

#endif // LIBABYSS_INPUTSTREAM_H
