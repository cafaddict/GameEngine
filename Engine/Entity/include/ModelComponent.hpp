#pragma once
#include "Component.hpp"
#include <ModelData.hpp>
#include <string>
namespace Engine {
    class ModelComponent : public Component
        {
            private:
            std::shared_ptr<const ModelData> modelData;
            public:
            ModelComponent(std::shared_ptr<const ModelData> modelData);
            ModelComponent();
            ~ModelComponent();
            void ModifyModelData();
            std::shared_ptr<const ModelData> GetModelData() const;

        };



    }