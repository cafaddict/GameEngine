#include "ModelComponent.hpp"

namespace Engine {
    ModelComponent::ModelComponent(std::shared_ptr<const ModelData> model_data) {
        ModelComponent::modelData = model_data;
        }
    ModelComponent::~ModelComponent() {

        }
    void ModelComponent::ModifyModelData() {
        if (!modelData.unique()) {
            modelData = std::make_shared<ModelData>(*modelData); // Copy-on-write
            }
        }

    std::shared_ptr<const ModelData> ModelComponent::GetModelData() const {
        return ModelComponent::modelData;
        }
    }