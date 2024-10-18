#include "ModelComponent.hpp"

namespace Engine {
ModelComponent::ModelComponent(std::shared_ptr<const Mesh> model_data) { ModelComponent::modelData = model_data; }
ModelComponent::~ModelComponent() {}
void ModelComponent::ModifyModelData() {
    if (!(modelData.use_count() == 1)) {
        modelData = std::make_shared<Mesh>(*modelData); // Copy-on-write
    }
}

void ModelComponent::SetModelData(std::shared_ptr<const Mesh> modelData) { this->modelData = modelData; }

std::shared_ptr<const Mesh> ModelComponent::GetModelData() const { return ModelComponent::modelData; }
} // namespace Engine