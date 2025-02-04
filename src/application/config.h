#ifndef MARMALADE_CONFIG_H
#define MARMALADE_CONFIG_H

#include <nlohmann/json.hpp>

#include <filesystem>

namespace Marmalade {

    struct Repository {
        std::string Name;
        std::string GitUrl{};
        int Depth{1};// Local repos do not work with shallow clone
    };

    struct EngineConfig {
        bool Viewports{true};
        std::vector<Repository> Repos{};
    };

    class Config {
    public:
        static EngineConfig engineConfig;

        static void SetConfigDirectory(bool sameDirConfig);
        static std::filesystem::path GetConfigDirectory();

        static void LoadEngineConfig();
        static void SaveEngineConfig();

    private:
        static std::filesystem::path _configDir;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::Repository, Name, GitUrl, Depth);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::EngineConfig, Viewports, Repos);
}

#endif
