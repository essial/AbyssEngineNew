#ifndef ABYSS_MPQ_H
#define ABYSS_MPQ_H

#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <StormLib.h>

namespace LibAbyss {

    class MPQStream;

    class MPQ {
    public:

        /// Proxy constructor that creates an MPQ based on the specified filename.
        /// \param mpqPath Path to the MPQ file to load.
        explicit MPQ(const std::filesystem::path &mpqPath);

        ~MPQ();

        bool HasFile(std::string_view fileName);
        MPQStream Load(std::string_view fileName);

    private:
        HANDLE _stormMpq;
        std::string _mpqPath;
    };

} // namespace LibAbyss

#endif // ABYSS_MPQ_H
