#pragma once
#include "Application.hpp"
#include "Log.hpp"

extern Engine::Application *Engine::CreateApplication();

int main(int argc, char **argv) {
    Engine::Log::Init();
    ENGINE_INFO("Initialized Log!");
#ifdef _WIN32
    system("..\\..\\resources\\shaders\\compile.bat");
#elif defined(__APPLE__)
    system("bash ../../resources/shaders/compile_mac.sh");
#else
    system("bash ../../resources/shaders/compile.sh");
#endif
    ENGINE_INFO("Compiled Shaders!");

    auto app = Engine::CreateApplication();
    try {
        app->run();
    } catch (const std::exception &e) {
        ENGINE_ERROR("{0}", e.what());
        // std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    delete app;
    return EXIT_SUCCESS;
}