#pragma once
#include "Component.hpp"
#include <vector>
#include <memory>
#include <string>
namespace Engine {
    class Entity
        {
            private:
            std::vector<std::shared_ptr<Component>> components;
            const std::string m_ID;
            public:
            Entity(std::string id) : m_ID(id) {};
            ~Entity() {};

            void AddComponent(std::shared_ptr<Component> component) {
                components.push_back(component);
                }

            std::string GetID() {
                return m_ID;
                }

            template<typename T>
            std::shared_ptr<T> GetComponent() {
                for (auto& component : components) {
                    if (auto comp = std::dynamic_pointer_cast<T>(component)) {
                        return comp;
                        }
                    }
                return nullptr;
                }
        };

    }