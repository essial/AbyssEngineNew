#ifndef ABYSS_INPUTSTREAM_H
#define ABYSS_INPUTSTREAM_H

#include <istream>
#include <streambuf>
#include <memory>

namespace LibAbyss {
class InputStream : public std::istream {
    public:
        explicit InputStream(std::unique_ptr<std::streambuf> buf) : _buf(std::move(buf)) {
            this->init(_buf.get());
        }
    private:
        std::unique_ptr<std::streambuf> _buf;
};
}

#endif // ABYSS_INPUTSTREAM_H
