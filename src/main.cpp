#include "application/application.h"
#include "application/config.h"

#include <iostream>

int main(int argc, char** argv) {
    bool sameDirConfig{false};

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--same-dir-config") {
            sameDirConfig = true;
        }
    }

    Marmalade::Config::SetConfigDirectory(sameDirConfig);
    std::cout << Marmalade::Config::GetConfigDirectory() << std::endl;

    Application& application = Application::GetInstance(1920, 1080, "Marmalade Engine");
    application.Initialise();

    while (!glfwWindowShouldClose(application.getWindow())) {
        application.Run();
    }

    application.Terminate();
    return 0;
}
