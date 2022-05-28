#pragma once

namespace Engine
{
  class Application
  {
  public:
    Application();
    virtual ~Application();

    void run();
    static int test();
  };
  int test();
  Application *CreateApplication();
} // namespace Engine