#include "scripthost.h"
#include "../engine/engine.h"
#include "../engine/mpqprovider.h"
#include "../engine/filesystemprovider.h"
#include <filesystem>
#include <spdlog/spdlog.h>
#include <absl/strings/ascii.h>

AbyssEngine::ScriptHost::ScriptHost(Engine *engine) : _lua(), _engine(engine) {

    _lua.open_libraries();

    _environment = sol::environment(_lua, sol::create, _lua.globals());

    _lua.set_exception_handler(
            [](lua_State *L, sol::optional<const std::exception &> maybe_exception, sol::string_view description) {
                // return LuaExceptionHandler(L, maybe_exception, description);
                std::string str(description);

                throw std::runtime_error(str.c_str());
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
}

std::tuple<sol::object, sol::object>
AbyssEngine::ScriptHost::LuaLoadString(const std::string_view str, const std::string &chunkName) {
    if (!str.empty() && str[0] == LUA_SIGNATURE[0])
        return std::make_tuple(sol::nil, sol::make_object(_lua, "Bytecode prohibited by Lua sandbox"));

    sol::load_result result = _lua.load(str, chunkName, sol::load_mode::text);

    if (!result.valid()) {
        sol::error err = result;
        return std::make_tuple(sol::nil, sol::make_object(_lua, err.what()));
    }

    sol::function func = result;
    _environment.set_on(func);

    return std::make_tuple(func, sol::nil);
}

std::tuple<sol::object, sol::object> AbyssEngine::ScriptHost::LuaLoadFile(const std::string &pathStr) {
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

sol::object AbyssEngine::ScriptHost::LuaDoFile(const std::string &path) {
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

void AbyssEngine::ScriptHost::LuaLog(const std::string &level, const std::string &message) {
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

    throw std::runtime_error("Unknown log level specified: " + level);
}

void AbyssEngine::ScriptHost::LuaSetBootText(const std::string &text) { _engine->SetBootText(text); }

void AbyssEngine::ScriptHost::LuaAddLoaderProvider(const std::string &providerType, const std::string &providerPath) {
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

    throw std::runtime_error("Unknown provider type: " + providerType);
}

void AbyssEngine::ScriptHost::LuaLoadPalette(const std::string &paletteName, const std::string &path) {
    bool isDat = !absl::AsciiStrToLower(path).ends_with(".pl2");
    std::filesystem::path filePath(path);
    auto stream = _engine->GetLoader().Load(filePath);
    auto palette = std::make_unique<LibAbyss::Palette>(stream, isDat);

    _engine->AddPalette(std::move(palette));
}

bool AbyssEngine::ScriptHost::LuaFileExists(const std::string &fileName) {
    auto path = std::filesystem::path(fileName);
    return _engine->GetLoader().FileExists(path);
}
