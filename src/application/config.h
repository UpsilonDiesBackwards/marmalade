#ifndef MARMALADE_CONFIG_H
#define MARMALADE_CONFIG_H

#include <filesystem>

namespace Marmalade {
    class Config {
    public:
        static void SetConfigDirectory(bool sameDirConfig);
        static std::filesystem::path GetConfigDirectory();

    private:
        static std::filesystem::path _configDir;
    };
}

#endif
