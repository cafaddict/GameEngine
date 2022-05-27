#pragma once

namespace Engine {
class Application {
public:
  Application();
  virtual ~Application();

  void run();
};

Application *CreateApplication();
} // namespace Engine