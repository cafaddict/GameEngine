#include <gtest/gtest.h>

#include <AssetManager.hpp>
#include <Component.hpp>
#include <Entity.hpp>
#include <EntityManager.hpp>
#include <ApplicationEvent.hpp>
#include <Event.hpp>
#include <glm/glm.hpp>

namespace {

class PositionComponent : public Engine::Component {
  public:
  explicit PositionComponent(glm::vec3 pos) : position(pos) {}
  glm::vec3 position;
};

class FakeAsset : public Engine::AssetData {
  public:
  bool Load(const std::string &path) override {
    lastPath = path;
    ++loadCount;
    return true;
  }

  std::string lastPath;
  int loadCount = 0;
};

class OtherFakeAsset : public Engine::AssetData {
  public:
  bool Load(const std::string &path) override {
    lastPath = path;
    ++loadCount;
    return true;
  }

  std::string lastPath;
  int loadCount = 0;
};

} // namespace

TEST(EcsTests, EntityManagerCreatesAndFindsEntities) {
  Engine::EntityManager manager;
  auto entity = manager.CreateEntity("hero");
  EXPECT_NE(entity, nullptr);
  EXPECT_EQ(manager.GetEntityCount(), 1u);
  EXPECT_EQ(manager.GetEntity("hero"), entity);
}

TEST(EcsTests, AddAndRetrieveComponent) {
  Engine::EntityManager manager;
  auto entity = manager.CreateEntity("hero");
  auto position = std::make_shared<PositionComponent>(glm::vec3(1.0f, 2.0f, 3.0f));
  entity->AddComponent(position);

  auto fetched = entity->GetComponent<PositionComponent>();
  ASSERT_NE(fetched, nullptr);
  EXPECT_EQ(fetched->position, glm::vec3(1.0f, 2.0f, 3.0f));
}

TEST(EcsTests, RemoveComponentByTypeClearsStoredComponent) {
  Engine::EntityManager manager;
  auto entity = manager.CreateEntity("hero");
  auto position = std::make_shared<PositionComponent>(glm::vec3(0.0f));
  entity->AddComponent(position);
  ASSERT_NE(entity->GetComponent<PositionComponent>(), nullptr);

  entity->RemoveComponent<PositionComponent>();
  EXPECT_EQ(entity->GetComponent<PositionComponent>(), nullptr);
}

TEST(AssetManagerTests, CachesAssetsByPathAndType) {
  Engine::AssetManager assetManager;

  auto first = assetManager.GetAsset<FakeAsset>("dummy/path.asset");
  ASSERT_NE(first, nullptr);
  EXPECT_EQ(first->lastPath, "dummy/path.asset");
  EXPECT_EQ(first->loadCount, 1);

  auto second = assetManager.GetAsset<FakeAsset>("dummy/path.asset");
  ASSERT_NE(second, nullptr);
  EXPECT_EQ(first, second) << "Expected cached shared_ptr for same path+type";
  EXPECT_EQ(second->loadCount, 1) << "Load should only be called once due to cache";
}

TEST(AssetManagerTests, CacheDistinguishesAssetTypesWithSamePath) {
  Engine::AssetManager assetManager;

  auto first = assetManager.GetAsset<FakeAsset>("shared/path.asset");
  auto second = assetManager.GetAsset<OtherFakeAsset>("shared/path.asset");

  ASSERT_NE(first, nullptr);
  ASSERT_NE(second, nullptr);
  EXPECT_NE(static_cast<void *>(first.get()), static_cast<void *>(second.get()))
      << "Different asset types sharing a path should not reuse the same instance";
  EXPECT_EQ(first->loadCount, 1);
  EXPECT_EQ(second->loadCount, 1);
}

TEST(EventDispatcherTests, DispatchSetsHandledOnMatchingType) {
  Engine::WindowResizeEvent resize{800, 600};
  Engine::EventDispatcher dispatcher(resize);
  bool called = false;
  unsigned int seenWidth = 0;
  unsigned int seenHeight = 0;

  bool dispatched = dispatcher.Dispatch<Engine::WindowResizeEvent>(
      [&](Engine::WindowResizeEvent &evt) {
        called = true;
        seenWidth = evt.GetWidth();
        seenHeight = evt.GetHeight();
        return true;
      });

  EXPECT_TRUE(dispatched);
  EXPECT_TRUE(called);
  EXPECT_EQ(seenWidth, 800u);
  EXPECT_EQ(seenHeight, 600u);
  EXPECT_TRUE(resize.IsHandled());
}

TEST(EventDispatcherTests, DispatchNonMatchingTypeReturnsFalse) {
  Engine::WindowResizeEvent resize{1024, 768};
  Engine::EventDispatcher dispatcher(resize);

  bool dispatched = dispatcher.Dispatch<Engine::WindowCloseEvent>([](Engine::WindowCloseEvent &) { return true; });

  EXPECT_FALSE(dispatched);
  EXPECT_FALSE(resize.IsHandled());
}
