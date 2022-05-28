// #include <Engine.hpp>
#include "gtest/gtest.h"
#include "Application.hpp"

TEST(EngineTest, test1)
{

  EXPECT_EQ(Engine::test(), 0);
  EXPECT_EQ(Engine::Application::test(), 100);
}