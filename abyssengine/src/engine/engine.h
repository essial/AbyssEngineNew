#ifndef ABYSS_ENGINE_H
#define ABYSS_ENGINE_H

#include "../common/inifile.h"
#include "../scripting/scripthost.h"
#include "../systemio/interface.h"
#include "libabyss/palette.h"
#include "loader.h"
#include <filesystem>
#include <map>
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
    void AddPalette(std::string_view paletteName, const LibAbyss::Palette &palette);
    Loader &GetLoader() { return _loader; }
    Common::INIFile &GetIniFile() { return _iniFile; }
    SystemIO::ISystemIO &GetSystemIO() { return *_systemIO; }

    static Engine *Get();

    const LibAbyss::Palette &GetPalette(std::string_view paletteName) const;

  private:
    const std::filesystem::path _basePath;
    Common::INIFile _iniFile;
    Loader _loader;
    bool _showSystemCursor;
    std::string _bootText;
    std::unique_ptr<AbyssEngine::SystemIO::ISystemIO> _systemIO;
    std::mutex _mutex;
    std::map<std::string, LibAbyss::Palette> _palettes;
};

} // namespace AbyssEngine

#endif // ABYSS_ENGINE_H
