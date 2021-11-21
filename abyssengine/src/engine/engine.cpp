#include "engine.h"
#include "filesystemprovider.h"
#include <spdlog/spdlog.h>

AbyssEngine::Engine::Engine(Common::INIFile iniFile, std::unique_ptr<SystemIO::ISystemIO> systemIo)
        : _iniFile(std::move(iniFile)), _systemIO(std::move(systemIo)), _loader(), _bootText(), _palettes() {

    SPDLOG_TRACE("creating engine");

    _systemIO->SetFullscreen(_iniFile.GetValueBool("Video", "FullScreen"));
}

void AbyssEngine::Engine::Run() {
    SPDLOG_TRACE("running engine");
    _loader.AddProvider(std::move(std::make_unique<FileSystemProvider>(std::filesystem::current_path())));
    std::thread scriptingThread([this] { ScriptingThreadBootstrap(); });
    _systemIO->RunMainLoop();
    scriptingThread.join();
}

AbyssEngine::Engine::~Engine() { SPDLOG_TRACE("destroying engine"); }

void AbyssEngine::Engine::ScriptingThreadBootstrap() {
    SPDLOG_TRACE("Scripting thread started");

    ScriptHost scriptHost(this);
    try {
        scriptHost.ExecuteString("dofile \"bootstrap\"");
    } catch (std::exception &ex) {
        SPDLOG_ERROR("Lua Error:\n{0}", ex.what());
        Stop();
    }

    SPDLOG_TRACE("Scripting thread finished");
}

void AbyssEngine::Engine::Stop() {
    std::lock_guard<std::mutex> guard(_mutex);

    _systemIO->Stop();
}

void AbyssEngine::Engine::ShowSystemCursor(bool show) {
    std::lock_guard<std::mutex> guard(_mutex);

    _showSystemCursor = show;
}

void AbyssEngine::Engine::SetBootText(std::string_view text) {
    std::lock_guard<std::mutex> guard(_mutex);

    _bootText = text;
}

void AbyssEngine::Engine::AddPalette(std::unique_ptr<LibAbyss::Palette> palette) {
    _palettes.push_back(std::move(palette));
}
