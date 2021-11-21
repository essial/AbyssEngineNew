#ifndef ABYSS_ENGINE_H
#define ABYSS_ENGINE_H

#include "../common/inifile.h"
#include "../scripting/scripthost.h"
#include "../systemio/interface.h"
#include "loader.h"
#include "libabyss/palette.h"
#include <filesystem>
#include <mutex>
#include <thread>

namespace AbyssEngine {

class Engine {
  public:
    Engine(Common::INIFile iniFile, std::unique_ptr<SystemIO::ISystemIO> systemIo);
    ~Engine();

    void Run();
    void Stop();
    void ScriptingThreadBootstrap();
    void ShowSystemCursor(bool show);
    void SetBootText(std::string_view text);
    void AddPalette(std::unique_ptr<LibAbyss::Palette> palette);
    Loader &GetLoader() { return _loader; }
    Common::INIFile &GetIniFile() { return _iniFile; }

  private:
    const std::filesystem::path _basePath;
    Common::INIFile _iniFile;
    Loader _loader;
    bool _showSystemCursor;
    std::string _bootText;
    std::unique_ptr<AbyssEngine::SystemIO::ISystemIO> _systemIO;
    std::mutex _mutex;
    std::vector<std::unique_ptr<LibAbyss::Palette>> _palettes;
};

} // namespace AbyssEngine

#endif // ABYSS_ENGINE_H
