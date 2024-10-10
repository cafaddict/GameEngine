#pragma once
#include "Component.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <typeindex>
#include <typeinfo>

namespace Engine {

class Entity {
    private:
    // Map to store components by type
    std::unordered_map<std::type_index, std::vector<std::shared_ptr<Component>>> components;
    const std::string m_ID;

    public:
    Entity(const std::string &id) : m_ID(id) {}
    ~Entity() = default;

    // Add a component to the entity
    void AddComponent(const std::shared_ptr<Component> &component) {
        components[typeid(*component)].push_back(component);
    }

    // Get the ID of the entity
    std::string GetID() const { return m_ID; }

    // Get a component of a specific type
    template <typename T> std::shared_ptr<T> GetComponent() const {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end() && !it->second.empty()) {
            // Return the first component of this type
            return std::dynamic_pointer_cast<T>(it->second.front());
        }
        return nullptr;
    }

    // Get all components of a specific type
    template <typename T> std::vector<std::shared_ptr<T>> GetComponents() const {
        std::vector<std::shared_ptr<T>> result;
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end()) {
            for (const auto &component : it->second) {
                if (auto comp = std::dynamic_pointer_cast<T>(component)) {
                    result.push_back(comp);
                }
            }
        }
        return result;
    }

    // Remove a component of a specific type
    template <typename T> void RemoveComponent() { components.erase(std::type_index(typeid(T))); }

    // Remove a specific component instance
    void RemoveComponent(const std::shared_ptr<Component> &component) {
        auto it = components.find(std::type_index(typeid(*component)));
        if (it != components.end()) {
            auto &vec = it->second;
            vec.erase(std::remove(vec.begin(), vec.end(), component), vec.end());
            // If the vector is empty, remove the entry from the map
            if (vec.empty()) {
                components.erase(it);
            }
        }
    }
};

} // namespace Engine
