#ifndef ABYSS_ENGINE_H
#define ABYSS_ENGINE_H

#include <filesystem>
#include <thread>
#include "../common/inifile.h"
#include "../systemio/interface.h"

namespace AbyssEngine {

    class Engine {
    public:
        Engine(AbyssEngine::Common::INIFile iniFile, std::unique_ptr<AbyssEngine::SystemIO::ISystemIO> systemIo);

        ~Engine();

        void Run();
        static void ScriptingThreadBootstrap();

    private:
        const std::filesystem::path _basePath;
        AbyssEngine::Common::INIFile _iniFile;
        std::unique_ptr<AbyssEngine::SystemIO::ISystemIO> _systemIO;
    };

}


#endif //ABYSS_ENGINE_H
