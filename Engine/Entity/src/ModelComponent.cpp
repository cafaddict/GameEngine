#include "ModelComponent.hpp"

namespace Engine
{
    ModelComponent::ModelComponent(std::shared_ptr<const ModelData> model_data)
    {
        ModelComponent::modelData = model_data;
    }
    ModelComponent::~ModelComponent()
    {
    }
    void ModelComponent::ModifyModelData()
    {
        if (!(modelData.use_count() == 1))
        {
            modelData = std::make_shared<ModelData>(*modelData); // Copy-on-write
        }
    }

    void ModelComponent::SetModelData(std::shared_ptr<const ModelData> modelData)
    {
        this->modelData = modelData;
    }

    std::shared_ptr<const ModelData> ModelComponent::GetModelData() const
    {
        return ModelComponent::modelData;
    }
}