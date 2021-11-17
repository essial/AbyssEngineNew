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

std::vector<std::string> LibAbyss::MPQ::FileList() {
    std::vector<std::string> result;

    if (!HasFile("(listfile)")) {
        SPDLOG_ERROR("MPQ does not contain a listfile.");
    }

    auto file = Load("(listfile)");
    std::istream stream(&file);

    while (!stream.eof()) {
        std::string line;
        stream >> line;
        result.push_back(line);
    }

    return result;
}