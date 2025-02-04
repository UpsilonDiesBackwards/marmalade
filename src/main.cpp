#include "application/application.h"

int main() {
    Application& application = Application::GetInstance(1920, 1080, "Engine Test");
    application.Initialise();

    while (!glfwWindowShouldClose(application.getWindow())) {
        application.Run();
    }

    application.Terminate();
    return 0;
}
