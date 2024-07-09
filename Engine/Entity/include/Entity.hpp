#pragma once
#include "Component.hpp"
#include <vector>
#include <memory>
namespace Engine {
    class Entity
        {
            private:
            std::vector<std::shared_ptr<Component>> components;
            public:
            Entity(/* args */) {};
            ~Entity() {};

            void AddComponent(std::shared_ptr<Component> component) {
                components.push_back(component);
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