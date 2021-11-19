#ifndef ABYSS_MPQPROVIDER_H
#define ABYSS_MPQPROVIDER_H

#include "provider.h"
#include <libabyss/mpq.h>

namespace AbyssEngine {

    class MPQProvider : public Provider {
    public:
        MPQProvider(std::filesystem::path &path);

    protected:
        bool Exists(std::filesystem::path &path) override;

        LibAbyss::InputStream Load(std::filesystem::path &path) override;

    private:
        LibAbyss::MPQ _mpq;
    };

}


#endif //ABYSS_MPQPROVIDER_H
