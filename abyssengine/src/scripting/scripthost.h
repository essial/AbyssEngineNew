#ifndef ABYSS_SCRIPTHOST_H
#define ABYSS_SCRIPTHOST_H

// This must be first for some reason---
#include <sol/sol.hpp>
// -------------------------------------

#include "../engine/provider.h"
#include "../node/sprite.h"
#include <filesystem>

namespace AbyssEngine {

class Engine;

class ScriptHost {
  public:
    explicit ScriptHost(Engine *engine);
    void ExecuteString(std::string_view code);
    void ExecuteFile(std::string_view path);

  private:
    Engine *_engine;

    sol::state _lua;
    sol::environment _environment;

    std::tuple<sol::object, sol::object> LuaLoadString(std::string_view str, std::string_view chunkName);
    std::tuple<sol::object, sol::object> LuaLoadFile(std::string_view pathStr);
    sol::object LuaDoFile(std::string_view path);

    // Script Functions -------------------

    void LuaFuncShutdown();
    std::string LuaGetConfig(std::string_view category, std::string_view value);
    void LuaShowSystemCursor(bool show);
    void LuaLog(std::string_view level, std::string_view message);
    void LuaAddLoaderProvider(std::string_view providerType, std::string_view providerPath);
    void LuaLoadPalette(std::string_view paletteName, std::string_view path);
    bool LuaFileExists(std::string_view fileName);
    std::unique_ptr<AbyssEngine::Sprite> LuaLoadSprite(std::string_view spritePath, std::string_view paletteName);
    void LuaSetCursor(Sprite &sprite, int offsetX, int offsetY);
    void LuaPlayVideo(std::string_view videoPath, bool wait);
    Node &LuaGetRootNode();
};

} // namespace AbyssEngine

#endif // ABYSS_SCRIPTHOST_H
