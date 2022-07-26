#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>
#include <memory>
#include <stdlib.h>

namespace Engine {
class Log {
 public:
  static void Init();
  /**
   * @brief Get the Core Logger object - Engine
   * @return std::shared_ptr<spdlog::logger>&
   */
  inline static std::shared_ptr<spdlog::logger> &GetCoreLogger() {
    return s_CoreLogger;
  }

  /**
   * @brief Get the Core Logger object - Client
   * @return std::share_ptr<spdlog::logger>&
   */
  inline static std::shared_ptr<spdlog::logger> &GetClientLogger() {
    return s_ClientLogger;
  }

 private:
  static std::shared_ptr<spdlog::logger> s_CoreLogger;
  static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

}  // namespace Engine

/**
 * @brief Engine log macros
 *
 */
#define ENGINE_TRACE(...) ::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ENGINE_INFO(...) ::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ENGINE_WARN(...) ::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...) ::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ENGINE_CRITICAL(...) \
  ::Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)

/**
 * @brief Client log macros
 *
 */
#define CLIENT_TRACE(...) ::Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CLIENT_INFO(...) ::Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define CLIENT_WARN(...) ::Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CLIENT_ERROR(...) ::Engine::Log::GetClientLogger()->error(__VA_ARGS__)
#define CLIENT_CRITICAL(...) \
  ::Engine::Log::GetClientLogger()->critical(__VA_ARGS__)

#define ENABLE_ASSERTS
#ifdef ENABLE_ASSERTS
#define CLIENT_ASSERT(x, ...)                             \
  {                                                       \
    if (!(x)) {                                           \
      CLIENT_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
      abort();                                            \
    }                                                     \
  }
#define ENGINE_ASSERT(x, ...)                             \
  {                                                       \
    if (!(x)) {                                           \
      ENGINE_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
      abort();                                            \
    }                                                     \
  }
#else
#define CLIENT_ASSERT(x, ...)
#define ENGINE_ASSERT(x, ...)
#endif