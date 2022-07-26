#include "Log.hpp"

namespace Engine
{
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    /**
     * @brief Initialize Log, Engine 이냐 Client에 따라 다른 looger 사용
     *
     */
    void Log::Init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        s_CoreLogger = spdlog::stdout_color_mt("Engine");
        s_CoreLogger->set_level(spdlog::level::trace);

        s_ClientLogger = spdlog::stdout_color_mt("Client");
        s_ClientLogger->set_level(spdlog::level::trace);
    }
}