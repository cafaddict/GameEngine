#include "Engine.hpp"
#include "gtest/gtest.h"

TEST(EngineTest, test1) {
  EXPECT_EQ(Engine::Engine_class::Engine_test(100), 100);
}