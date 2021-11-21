#ifndef ABYSS_SCRIPTHOST_H
#define ABYSS_SCRIPTHOST_H

#include <sol/sol.hpp>

#include "../engine/provider.h"
#include <filesystem>

namespace AbyssEngine {

class Engine;

class ScriptHost {
  public:
    ScriptHost(Engine *engine);
    void ExecuteString(std::string_view code);

  private:
    Engine *_engine;

    sol::state _lua;
    sol::environment _environment;

    std::tuple<sol::object, sol::object> LuaLoadString(const std::string_view str, const std::string &chunkName);
    std::tuple<sol::object, sol::object> LuaLoadFile(const std::string &pathStr);
    sol::object LuaDoFile(const std::string &path);

    // Script Functions -------------------

    void LuaFuncShutdown();
    std::string LuaGetConfig(std::string_view category, std::string_view value);
    void LuaShowSystemCursor(bool show);
    void LuaLog(const std::string& level, const std::string& message);
    void LuaSetBootText(const std::string& text);
    void LuaAddLoaderProvider(const std::string& providerType, const std::string& providerPath);
    void LuaLoadPalette(const std::string& paletteName, const std::string& path);
    bool LuaFileExists(const std::string& fileName);
};

} // namespace AbyssEngine

#endif // ABYSS_SCRIPTHOST_H
