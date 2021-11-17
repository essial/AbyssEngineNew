#include "../include/libabyss/mpq.h"
#include "../include/libabyss/mpqstream.h"
#include <spdlog/spdlog.h>

LibAbyss::MPQ::MPQ(const std::filesystem::path &mpqPath) : _mpqPath(std::filesystem::absolute(mpqPath).c_str()) {
    if (!SFileOpenArchive(_mpqPath.c_str(), 0, STREAM_PROVIDER_FLAT | BASE_PROVIDER_FILE | STREAM_FLAG_READ_ONLY,
                          &_stormMpq)) {
        throw std::runtime_error("Error occurred while loading MPQ");
    }

    SPDLOG_INFO("Loaded {0}", _mpqPath);
}


LibAbyss::MPQ::~MPQ() {
    SPDLOG_INFO("Unloaded {0}", _mpqPath);
}

LibAbyss::MPQStream LibAbyss::MPQ::Load(std::string_view fileName) {
    return {_stormMpq, fileName};
}

bool LibAbyss::MPQ::HasFile(std::string_view fileName) {
    return SFileHasFile(_stormMpq, (const char*)fileName.data());
}
