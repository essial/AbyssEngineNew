#include "config.h"
#include "libabyss/mpqstream.h"
#include <argh.h>
#include <cstdlib>
#include <filesystem>
#include <spdlog/spdlog.h>

void ExtractMPQ(std::string_view mpqFile, const std::filesystem::path &outputPath) {
    SPDLOG_INFO("Extracting {0} to {1}", mpqFile, outputPath.string());

    if (!exists(outputPath))
        std::filesystem::create_directories(outputPath);
}

void ListMPQ(std::string_view mpqFile) {
    std::filesystem::path mpqPath(mpqFile);
    mpqPath = std::filesystem::absolute(mpqPath);

    SPDLOG_INFO("Listing contents of {0}", mpqPath.string());

    LibAbyss::MPQ mpq(mpqPath);

    if (!mpq.HasFile("(listfile)")) {
        SPDLOG_ERROR("MPQ does not contain a listfile.");
    }

    for (const auto& line : mpq.FileList()) {
        SPDLOG_INFO(line);
    }

}

void ListMPQContents(std::string_view mpqFile) { SPDLOG_INFO("Listing contents of {0}:", mpqFile); }

int main(int, char *argv[]) {
    SPDLOG_INFO(ABYSS_VERSION_STRING);

    argh::parser commandLine(argv);

    if (commandLine[{"-v", "--verbose"}]) {
        spdlog::set_level(spdlog::level::trace);
        SPDLOG_INFO("Verbose logging enabled");
    } else {
        spdlog::set_level(spdlog::level::info);
    }

    {
        std::string extractPath;
        if (commandLine({"-e", "--extract"}) >> extractPath) {
            std::filesystem::path outPath(commandLine({"-o", "--outpath"}).str());
            ExtractMPQ(extractPath, absolute(outPath));

            return EXIT_SUCCESS;
        }
    }

    {
        std::string path;
        if (commandLine({"-l", "--list"}) >> path) {
            ListMPQ(path);

            return EXIT_SUCCESS;
        }
    }

    {
        std::string extractPath;
        if (commandLine({"-l", "--list"}) >> extractPath) {
            ListMPQContents(extractPath);

            return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}
