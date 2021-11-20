#include "scripthost.h"
#include "../engine/engine.h"
#include <filesystem>
#include <spdlog/spdlog.h>

AbyssEngine::ScriptHost::ScriptHost(Engine *engine) : _lua(), _environment(_lua, sol::create), _engine(engine) {

    _lua.open_libraries( );
    _lua.set_exception_handler([](lua_State *L, sol::optional<const std::exception &> maybe_exception,
                                  sol::string_view description) {
        //return LuaExceptionHandler(L, maybe_exception, description);
        std::string str(description);

        throw std::runtime_error(str.c_str());
        return 0;
    });

    _environment["_G"] = _environment;

    auto whitelisted = {
            "assert", "error", "ipairs", "next", "pairs", "pcall", "print", "select", "tonumber", "tostring", "type",
            "unpack", "_VERSION", "xpcall",
            // These functions are unsafe as they can bypass or change metatables, but they are required to implement classes.
            "rawequal", "rawget", "rawset", "setmetatable"
    };

    for (const auto &name: whitelisted) {
        _environment[name] = _lua[name];
    }

    std::vector<std::string> safeLibraries = {
            "coroutine", "string", "table", "math"};

    for (const auto &name : safeLibraries) {
        sol::table copy(_lua, sol::create);

        for (const auto& pair : _lua[name].tbl) {
            copy[pair.first] = pair.second;
        }
        _environment[name] = copy;
    }

    sol::table os(_lua, sol::create);
    os["clock"] = _lua["os"]["clock"];
    os["date"] = _lua["os"]["date"];
    os["difftime"] = _lua["os"]["difftime"];
    os["time"] = _lua["os"]["time"];
    _environment["os"] = os;

    lua_rawgeti(_lua, LUA_REGISTRYINDEX, _environment.registry_index());
    lua_rawseti(_lua, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);

    _environment.set_function("loadstring", &ScriptHost::LuaLoadString, this);
    _environment.set_function("loadfile", &ScriptHost::LuaLoadFile, this);
    _environment.set_function("dofile", &ScriptHost::LuaDoFile, this);
    _environment.set_function("require", &ScriptHost::LuaLoadFile, this);

    _environment.set_function("shutdown", &ScriptHost::LuaFuncShutdown, this);
    _environment.set_function("getConfig", &ScriptHost::LuaGetConfig, this);
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
