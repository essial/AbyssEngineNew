#ifndef ABYSS_ENGINE_H
#define ABYSS_ENGINE_H

#include <filesystem>
#include <thread>
#include <mutex>
#include "../common/inifile.h"
#include "../systemio/interface.h"
#include "../scripting/scripthost.h"
#include "loader.h"

namespace AbyssEngine {

    class Engine {
    public:
        Engine(Common::INIFile iniFile, std::unique_ptr<SystemIO::ISystemIO> systemIo);
        ~Engine();

        void Run();
        void Stop();
        void ScriptingThreadBootstrap();
        Loader &GetLoader() { return _loader; }

    private:
        const std::filesystem::path _basePath;
        Common::INIFile _iniFile;
        Loader _loader;
        std::unique_ptr<AbyssEngine::SystemIO::ISystemIO> _systemIO;
        static std::mutex _mutex;


    };

}


#endif //ABYSS_ENGINE_H
