#ifndef ABYSS_MPQ_H
#define ABYSS_MPQ_H

#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <StormLib.h>
#include <istream>
#include <filesystem>
#include <libabyss/inputstream.h>

namespace LibAbyss {

    class MPQStream;

    class MPQ {
    public:

        /// Proxy constructor that creates an MPQ based on the specified filename.
        /// \param mpqPath Path to the MPQ file to load.
        explicit MPQ(const std::filesystem::path &mpqPath);

        ~MPQ();

        bool HasFile(const std::string &fileName);
        InputStream Load(const std::string &fileName);
        std::vector<std::string> FileList();

    private:
        HANDLE _stormMpq;
        std::string _mpqPath;

        std::string FixPath(std::string str);
    };

} // namespace LibAbyss

#endif // ABYSS_MPQ_H
