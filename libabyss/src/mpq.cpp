#include "../include/libabyss/mpq.h"
#include "../include/libabyss/mpqstream.h"
#include <memory>
#include <spdlog/spdlog.h>

LibAbyss::MPQ::MPQ(const std::filesystem::path &mpqPath) : _mpqPath(std::filesystem::absolute(mpqPath).string()) {
    if (!SFileOpenArchive(_mpqPath.c_str(), 0, STREAM_PROVIDER_FLAT | BASE_PROVIDER_FILE | STREAM_FLAG_READ_ONLY,
                          &_stormMpq)) {
        throw std::runtime_error("Error occurred while loading MPQ");
    }
}


LibAbyss::MPQ::~MPQ() {
    SFileCloseArchive(_stormMpq);
}

LibAbyss::InputStream LibAbyss::MPQ::Load(const std::string& fileName) {
    return LibAbyss::InputStream(std::make_unique<LibAbyss::MPQStream>(_stormMpq, FixPath(fileName)));
}

bool LibAbyss::MPQ::HasFile(const std::string &fileName) {
    return SFileHasFile(_stormMpq, FixPath(fileName).c_str());
}

std::vector<std::string> LibAbyss::MPQ::FileList() {
    std::vector<std::string> result;

    if (!HasFile("(listfile)")) {
        SPDLOG_ERROR("MPQ does not contain a listfile.");
    }

    auto stream = Load("(listfile)");

    while (!stream.eof()) {
        std::string line;
        stream >> line;
        result.push_back(line);
    }

    return result;
}

std::string LibAbyss::MPQ::FixPath(std::string str) {
    std::string result(str);
    std::replace(result.begin(), result.end(), '/', '\\');
    if (result.starts_with('\\')) {
        result = result.substr(1);
    }

    return result;
}
