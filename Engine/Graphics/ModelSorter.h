#pragma once

#include <vector>
#include <map>

#include "Model.h"
#include "Skeleton.h"
#include "Math/Camera.h"

class ModelSorter {
public:
    struct Key {
        Model* model;
        Skeleton* skeleton;
        bool operator<(const Key& key) const;
    };

    void Sort(const Camera& camera);

    const std::map<Key, std::vector<ModelInstance*>>& GetModelInstanceMap() const { return modelInstanceMap_; }
    const std::vector<ModelInstance*>& GetDrawModels() const { return drawModels_; }

private:
    std::map<Key, std::vector<ModelInstance*>> modelInstanceMap_;
    std::vector<ModelInstance*> drawModels_;
};