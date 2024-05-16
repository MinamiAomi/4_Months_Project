#include "ModelSorter.h"

void ModelSorter::Sort(const Camera& camera) {
    modelInstanceMap_.clear();
    drawModels_.clear();
    camera;
    auto& instanceList = ModelInstance::GetInstanceList();
    size_t numDrawModels = 0;
    for (auto& instance : instanceList) {
        auto model = instance->GetModel().get();
        auto skeleton = instance->GetSkeleton().get();
        if (!(instance->IsActive() && model != nullptr)) { continue; }
        Key key{ model, skeleton };
        modelInstanceMap_[key].emplace_back(instance);
        ++numDrawModels;
    }
    drawModels_.reserve(numDrawModels);
    for (auto& modelInstance : modelInstanceMap_) {
        for (auto& instance : modelInstance.second) {
            drawModels_.emplace_back(instance);
        }
    }
}

bool ModelSorter::Key::operator<(const Key& key) const {
    return std::tie(model, skeleton) < std::tie(key.model, key.skeleton);
}
