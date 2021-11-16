#include "engine.h"
#include <spdlog/spdlog.h>


AbyssEngine::Engine::Engine(AbyssEngine::Common::INIFile &iniFile, std::unique_ptr<AbyssEngine::SystemIO::ISystemIO> systemIo) :
        _iniFile(std::move(iniFile)), _systemIO(std::move(systemIo)) {
    SPDLOG_TRACE("creating engine");

    _systemIO->SetFullscreen(_iniFile.GetValueBool("Video", "FullScreen"));
}


void AbyssEngine::Engine::Run() {
    SPDLOG_TRACE("running engine");
    std::thread _scriptingThread(Engine::ScriptingThreadBootstrap);
    _systemIO->RunMainLoop();
    _scriptingThread.join();
}

AbyssEngine::Engine::~Engine() {
    SPDLOG_TRACE("destroying engine");
}

void AbyssEngine::Engine::ScriptingThreadBootstrap() {
    SPDLOG_TRACE("Scripting thread started");


    SPDLOG_TRACE("Scripting thread finished");
}
