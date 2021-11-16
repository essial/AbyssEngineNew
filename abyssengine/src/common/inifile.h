#ifndef ABYSS_INIFILE_H
#define ABYSS_INIFILE_H


#include <filesystem>
#include <string>
#include <absl/container/flat_hash_map.h>

namespace AbyssEngine::Common {
    class INIFile {
    public:
        explicit INIFile(const std::filesystem::path &iniFilePath);

        std::string GetValue(const std::string &category, const std::string &name);

        bool GetValueBool(const std::string &category, const std::string &name);

    private:
        absl::flat_hash_map<std::string, absl::flat_hash_map<std::string, std::string>> _values;
    };

}

#endif //ABYSS_INIFILE_H
