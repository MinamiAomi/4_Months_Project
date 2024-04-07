#pragma once

#include <map>
#include <vector>
#include <string>
#include <optional>

#include "Math/MathUtils.h"
#include "Math/Transform.h"

template<typename T>
struct Keyframe {
    float time;
    T value;
};

template<typename T>
struct AnimationCurve {
    std::vector<Keyframe<T>> keyframes;
};

struct NodeAnimation {
    AnimationCurve<Vector3> translate;
    AnimationCurve<Quaternion> rotate;
    AnimationCurve<Vector3> scale;
};

struct Animation {
    float duration;
    std::map<std::string, NodeAnimation> nodeAnimations;
};


struct Joint {
    Transform transform;
    Matrix4x4 localMatrix;
    Matrix4x4 skeletonSpaceMatrix;
    std::string name;
    std::vector<int32_t> children;
    int32_t index;
    std::optional<int32_t> parent;
};

struct Skeleton {
    int32_t root;
    std::map<std::string, int32_t> jointMap;
    std::vector<Joint> joints;
};