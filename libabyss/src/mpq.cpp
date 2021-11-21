#include "../include/libabyss/mpq.h"
#include "../include/libabyss/mpqstream.h"
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <absl/strings/str_format.h>

LibAbyss::MPQ::MPQ(const std::filesystem::path &mpqPath) : _mpqPath(std::filesystem::absolute(mpqPath).string()) {
    if (!SFileOpenArchive(_mpqPath.c_str(), 0, STREAM_PROVIDER_FLAT | BASE_PROVIDER_FILE | STREAM_FLAG_READ_ONLY,
                          &_stormMpq)) {
        throw std::runtime_error(absl::StrFormat("Error occurred while opening MPQ %s", mpqPath.string()));
    }

    SPDLOG_INFO("Loaded {0}", _mpqPath);
}


LibAbyss::MPQ::~MPQ() {
    SFileCloseArchive(_stormMpq);
    SPDLOG_INFO("Unloaded {0}", _mpqPath);
}

LibAbyss::InputStream LibAbyss::MPQ::Load(std::string_view fileName) {
    return LibAbyss::InputStream(std::make_unique<LibAbyss::MPQStream>(_stormMpq, fileName));
}

bool LibAbyss::MPQ::HasFile(std::string_view fileName) {
    return SFileHasFile(_stormMpq, std::string(fileName).c_str());
}

std::vector<std::string> LibAbyss::MPQ::FileList() {
    std::vector<std::string> result;

    if (!HasFile("(listfile)")) {
        SPDLOG_ERROR("MPQ does not contain a listfile.");
    }

    auto stream = Load("(listfile)");

    std::string line;
    while (true) {
        std::getline(stream, line);
        if (stream.eof()) {
            break;
        }
        result.push_back(line);
    }

    return result;
}
