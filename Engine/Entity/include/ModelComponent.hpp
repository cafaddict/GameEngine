#pragma once
#include "Component.hpp"
#include <ModelData.hpp>
#include <string>
namespace Engine {
class ModelComponent : public Component {
    private:
    std::shared_ptr<const Mesh> modelData;

    public:
    ModelComponent(std::shared_ptr<const Mesh> modelData);
    ModelComponent();
    ~ModelComponent();
    void ModifyModelData();
    void SetModelData(std::shared_ptr<const Mesh> modelData);
    std::shared_ptr<const Mesh> GetModelData() const;
};

} // namespace Engine