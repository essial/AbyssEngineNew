#ifndef ABYSS_ENGINE_H
#define ABYSS_ENGINE_H

#include <filesystem>
#include <thread>
#include "../common/inifile.h"
#include "../systemio/interface.h"

namespace AbyssEngine::Engine {

    class Engine {
    public:
        Engine(std::unique_ptr<AbyssEngine::Common::INIFile> &iniFile, std::unique_ptr<AbyssEngine::SystemIO::ISystemIO> &systemIo);

        ~Engine();

        void Run();
        static void ScriptingThreadBootstrap();

    private:
        const std::filesystem::path _basePath;
        std::unique_ptr<AbyssEngine::SystemIO::ISystemIO> _systemIO;
        std::unique_ptr<AbyssEngine::Common::INIFile> _iniFile;
    };

}


#endif //ABYSS_ENGINE_H
