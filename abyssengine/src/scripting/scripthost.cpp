#include "scripthost.h"
#include "../engine/filesystemprovider.h"
#include "../engine/mpqprovider.h"
#include "../engine/spritefont.h"
#include "../engine/engine.h"
#include <absl/strings/ascii.h>
#include <filesystem>
#include <spdlog/spdlog.h>

AbyssEngine::ScriptHost::ScriptHost(Engine *engine) : _lua(), _engine(engine) {

    _lua.open_libraries();

    _environment = sol::environment(_lua, sol::create, _lua.globals());

    _lua.set_exception_handler([](lua_State *L, sol::optional<const std::exception &> maybe_exception, sol::string_view description) {
        // return LuaExceptionHandler(L, maybe_exception, description);

        throw std::runtime_error(std::string(description));
        return 0;
    });

    // Overload loading functions
    _environment.set_function("loadstring", &ScriptHost::LuaLoadString, this);
    _environment.set_function("loadfile", &ScriptHost::LuaLoadFile, this);
    _environment.set_function("dofile", &ScriptHost::LuaDoFile, this);
    _environment.set_function("require", &ScriptHost::LuaLoadFile, this);

    // Engine Functions
    _environment.set_function("shutdown", &ScriptHost::LuaFuncShutdown, this);
    _environment.set_function("getConfig", &ScriptHost::LuaGetConfig, this);
    _environment.set_function("showSystemCursor", &ScriptHost::LuaShowSystemCursor, this);
    _environment.set_function("log", &ScriptHost::LuaLog, this);
    _environment.set_function("setBootText", &ScriptHost::LuaSetBootText, this);
    _environment.set_function("addLoaderProvider", &ScriptHost::LuaAddLoaderProvider, this);
    _environment.set_function("loadPalette", &ScriptHost::LuaLoadPalette, this);
    _environment.set_function("fileExists", &ScriptHost::LuaFileExists, this);

    // User Types
    _environment.new_usertype<AbyssEngine::SpriteFont>("SpriteFont", "new", sol::constructors<SpriteFont(std::string, std::string)>());
}

std::tuple<sol::object, sol::object> AbyssEngine::ScriptHost::LuaLoadString(const std::string_view str, std::string_view chunkName) {
    if (!str.empty() && str[0] == LUA_SIGNATURE[0])
        return std::make_tuple(sol::nil, sol::make_object(_lua, "Bytecode prohibited by Lua sandbox"));

    sol::load_result result = _lua.load(str, std::string(chunkName), sol::load_mode::text);

    if (!result.valid()) {
        sol::error err = result;
        return std::make_tuple(sol::nil, sol::make_object(_lua, err.what()));
    }

    sol::function func = result;
    _environment.set_on(func);

    return std::make_tuple(func, sol::nil);
}

std::tuple<sol::object, sol::object> AbyssEngine::ScriptHost::LuaLoadFile(std::string_view pathStr) {
    std::filesystem::path path(pathStr);

    if (!path.has_extension())
        path += ".lua";

    // TODO: Ensure we haven't already loaded this

    if (!_engine->GetLoader().FileExists(path))
        return std::make_tuple(sol::nil, sol::make_object(_lua, "Path is not allowed by the Lua sandbox"));

    auto stream = _engine->GetLoader().Load(path);

    std::istreambuf_iterator<char> eos;
    std::string str(std::istreambuf_iterator<char>(stream), eos);

    auto ret = LuaLoadString(str, "@" + path.string());

    if (std::get<0>(ret) == sol::nil) {
        throw sol::error(std::get<1>(ret).as<std::string>());
    }

    sol::unsafe_function func = std::get<0>(ret);
    return func();
}

void AbyssEngine::ScriptHost::ExecuteString(std::string_view code) {
    auto result = _lua.script(code, _environment);
    if (!result.valid()) {
        const sol::error error = result;
        const std::string errorMessage = error.what();
        throw std::runtime_error(errorMessage);
    }
}

sol::object AbyssEngine::ScriptHost::LuaDoFile(std::string_view path) {
    std::tuple<sol::object, sol::object> ret = LuaLoadFile(path);
    if (std::get<0>(ret) == sol::nil) {
        throw sol::error(std::get<1>(ret).as<std::string>());
    }

    sol::unsafe_function func = std::get<0>(ret);
    return func();
}

void AbyssEngine::ScriptHost::LuaFuncShutdown() {
    SPDLOG_INFO("Shutting down engine");
    _engine->Stop();
}

std::string AbyssEngine::ScriptHost::LuaGetConfig(std::string_view category, std::string_view value) {
    return _engine->GetIniFile().GetValue(category, value);
}

void AbyssEngine::ScriptHost::LuaShowSystemCursor(bool show) { _engine->ShowSystemCursor(show); }

void AbyssEngine::ScriptHost::LuaLog(std::string_view level, std::string_view message) {
    if (level == "info") {
        SPDLOG_INFO(message);
        return;
    }

    if (level == "error") {
        SPDLOG_ERROR(message);
        return;
    }

    if (level == "fatal") {
        SPDLOG_CRITICAL(message);
        return;
    }

    if (level == "warn") {
        SPDLOG_WARN(message);
        return;
    }

    if (level == "debug") {
        SPDLOG_DEBUG(message);
        return;
    }

    if (level == "trace") {
        SPDLOG_TRACE(message);
        return;
    }

    throw std::runtime_error("Unknown log level specified: " + std::string(level));
}

void AbyssEngine::ScriptHost::LuaSetBootText(std::string_view text) { _engine->SetBootText(text); }

void AbyssEngine::ScriptHost::LuaAddLoaderProvider(std::string_view providerType, std::string_view providerPath) {
    if (providerType == "mpq") {
        auto path = std::filesystem::path(providerPath);
        auto provider = std::make_unique<AbyssEngine::MPQProvider>(path);
        _engine->GetLoader().AddProvider(std::move(provider));

        return;
    }

    if (providerType == "filesystem") {
        auto path = std::filesystem::path(providerPath);
        auto provider = std::make_unique<AbyssEngine::FileSystemProvider>(path);
        _engine->GetLoader().AddProvider(std::move(provider));

        return;
    }

    throw std::runtime_error("Unknown provider type: " + std::string(providerType));
}

void AbyssEngine::ScriptHost::LuaLoadPalette(std::string_view paletteName, std::string_view path) {
    bool isDat = !absl::AsciiStrToLower(path).ends_with(".pl2");
    std::filesystem::path filePath(path);
    auto stream = _engine->GetLoader().Load(filePath);
    LibAbyss::Palette palette(stream, isDat);
    _engine->AddPalette(paletteName, palette);
}

bool AbyssEngine::ScriptHost::LuaFileExists(std::string_view fileName) {
    auto path = std::filesystem::path(fileName);
    return _engine->GetLoader().FileExists(path);
}
//AbyssEngine::Sprite* AbyssEngine::ScriptHost::LuaLoadSprite(std::string_view spritePath, std::string_view paletteName) {
//    if (absl::AsciiStrToLower(spritePath).ends_with(".dc6")) {
//        auto palette = _engine->GetPalette(paletteName);
//        return new DC6Sprite(spritePath, palette);
//    }
//
//
//}
