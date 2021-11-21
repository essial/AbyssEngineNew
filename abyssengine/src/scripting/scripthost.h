#ifndef ABYSS_SCRIPTHOST_H
#define ABYSS_SCRIPTHOST_H

#include <sol/sol.hpp>
#include <filesystem>
#include "../engine/provider.h"

namespace AbyssEngine {

    class Engine;

    class ScriptHost {
    public:
        ScriptHost(Engine *engine);
        void ExecuteString(std::string_view code);


    private:

        sol::state _lua;
        sol::environment _environment;
        Engine *_engine;

        std::tuple<sol::object, sol::object> LuaLoadString(const std::string_view str, const std::string &chunkName);
        std::tuple<sol::object, sol::object> LuaLoadFile(const std::string &pathStr);
        sol::object LuaDoFile(const std::string &path);

        // Script Functions -------------------

        void LuaFuncShutdown();
        std::string LuaGetConfig(std::string_view category, std::string_view value);
    };

}


#endif //ABYSS_SCRIPTHOST_H
