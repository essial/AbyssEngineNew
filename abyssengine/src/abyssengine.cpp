#include <string>
#include <filesystem>
#include "engine/engine.h"
#include <config.h>
#include <memory>
#include "systemio/sdl2/sdl2systemio.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::filesystem::path GetConfigPath(std::string_view exePath);

int main(int, char *argv[]) {
    SPDLOG_INFO(ABYSS_VERSION_STRING);

#ifndef NDEBUG
    spdlog::set_level(spdlog::level::trace);
#endif


    try {
        auto configPath = GetConfigPath(argv[0]);
        auto iniFile = std::make_unique<AbyssEngine::Common::INIFile>(configPath);
        std::unique_ptr<AbyssEngine::SystemIO::ISystemIO> systemIo = std::make_unique<AbyssEngine::SystemIO::SDL2::SDL2SystemIO>();

        auto engine = std::make_unique<AbyssEngine::Engine::Engine>(iniFile, systemIo);
        engine->Run();
    } catch (std::exception &ex) {
        SPDLOG_CRITICAL(ex.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

std::filesystem::path GetConfigPath(std::string_view exePath) {
    auto testPath = std::filesystem::current_path() / "config.ini";

    // Test working directory path
    if (exists(testPath))
        return testPath.remove_filename();

    // Test executable path
    testPath = std::filesystem::path(exePath).remove_filename() / "config.ini";
    if (exists(testPath))
        return testPath.remove_filename();

#ifdef __APPLE__
    // Test OSX app package path
    testPath = std::filesystem::current_path() / ".." / ".." / ".." / "config.ini";

    if (exists(testPath))
        return testPath.remove_filename();
#endif
    return {};
}
