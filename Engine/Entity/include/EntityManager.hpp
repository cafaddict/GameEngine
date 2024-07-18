#pragma once
#include <queue>
#include <Entity.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#define MAX_ENTITIES = 1000;
namespace Engine {
    class EntityManager {
        private:
        std::unordered_map<std::string, std::shared_ptr<Entity>> entities;

        public:
        EntityManager() {}
        ~EntityManager() {}

        std::shared_ptr<Entity> CreateEntity(const std::string& id) {
            auto entity = std::make_shared<Entity>();
            entities[id] = entity;
            return entity;
            }

        std::shared_ptr<Entity> GetEntity(const std::string& id) {
            auto it = entities.find(id);
            if (it != entities.end()) {
                return it->second;
                }
            return nullptr;
            }

        void RemoveEntity(const std::string& id) {
            entities.erase(id);
            }

        std::vector<std::shared_ptr<Entity>> GetAllEntities() const {
            std::vector<std::shared_ptr<Entity>> allEntities;
            for (const auto& pair : entities) {
                allEntities.push_back(pair.second);
                }
            return allEntities;
            }

        size_t GetEntityCount() {
            return entities.size();
            }
        };
    }