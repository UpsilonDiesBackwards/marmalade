#ifndef MARMALADE_CONFIG_H
#define MARMALADE_CONFIG_H

#include <nlohmann/json.hpp>

#include <filesystem>

namespace Marmalade {

    struct EngineConfig {
        bool Viewports{true};
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

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::EngineConfig, Viewports);
}

#endif
