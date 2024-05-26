#include "Skeleton.h"

#include <cassert>

#include "Model.h"
#include "RenderManager.h"
#include "SkinningManager.h"

Skeleton::~Skeleton() {
    RenderManager::GetInstance()->GetSkinningManager().Remove(this);
}

void Skeleton::Create(const std::shared_ptr<Model>& model) {
    assert(model);
    joints_.clear();
    jointMap_.clear();
    root_ = CreateJoint(model->GetRootNode(), {}, joints_);
    for (const Joint& joint : joints_) {
        jointMap_.emplace(joint.name, joint.index);
    }
    RenderManager::GetInstance()->GetSkinningManager().Add(this, model);
    updated_ = true;
}

int32_t Skeleton::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
    Joint joint;
    joint.name = node.name;
    joint.localMatrix = node.localMatrix;
    joint.skeletonSpaceMatrix = Matrix4x4::identity;
    joint.transform.translate = node.transform.translate;
    joint.transform.rotate = node.transform.rotate;
    joint.transform.scale = node.transform.scale;
    joint.index = int32_t(joints.size());
    joint.parent = parent;
    joints.push_back(joint);
    for (const Node& child : node.children) {
        int32_t childIndex = CreateJoint(child, joint.index, joints);
        joints[joint.index].children.push_back(childIndex);
    }
    return joint.index;
}


void Skeleton::Update() {
    // すべてのjointを更新。親が若いので通常ループで処理可能になっている
    for (Joint& joint : joints_) {
        joint.localMatrix = Matrix4x4::MakeAffineTransform(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
        // 親がいれば親の行列をかける
        if (joint.parent) {
            joint.skeletonSpaceMatrix = joint.localMatrix * joints_[*joint.parent].skeletonSpaceMatrix;
        }
        else {
            joint.skeletonSpaceMatrix = joint.localMatrix;
        }
    }
    updated_ = true;
}

void Skeleton::DebugDraw(const Matrix4x4& worldMatrix, const Vector4& color) {
    auto& lineDrawer = RenderManager::GetInstance()->GetLineDrawer();
    for (Joint& joint : joints_) {
        if (joint.parent) {
            Vector3 start = joint.skeletonSpaceMatrix.GetTranslate() * Matrix4x4::MakeScaling({ -1.0f, 1.0f, -1.0f }) * worldMatrix;
            Vector3 end = joints_[*joint.parent].skeletonSpaceMatrix.GetTranslate() * Matrix4x4::MakeScaling({ -1.0f, 1.0f, -1.0f }) * worldMatrix;
            lineDrawer.AddLine(start, end, color);
        }
    }
}


void Skeleton::ApplyAnimation(const AnimationSet& animation, float animationTime) {
    for (Joint& joint : joints_) {
        if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
            const NodeAnimation& rootNodeAnimation = (*it).second;
            joint.transform.translate = CalculateValue(rootNodeAnimation.translate, animationTime);
            joint.transform.rotate = CalculateValue(rootNodeAnimation.rotate, animationTime);
            joint.transform.scale = CalculateValue(rootNodeAnimation.scale, animationTime);
        }
    }
}

void Skeleton::ApplyAnimationTransition(const AnimationSet& from, float fromTime, const AnimationSet& to, float toTime, float animationTime) {
    for (Joint& joint : joints_) {
        auto fromIt = from.nodeAnimations.find(joint.name);
        auto toIt = to.nodeAnimations.find(joint.name);
        if (fromIt != from.nodeAnimations.end() && toIt != to.nodeAnimations.end()) {
            const NodeAnimation& fromNodeAnimation = (*fromIt).second;
            const NodeAnimation& toNodeAnimation = (*toIt).second;
            joint.transform.translate = Vector3::Lerp(animationTime, CalculateValue(fromNodeAnimation.translate, fromTime), CalculateValue(toNodeAnimation.translate, toTime));
            joint.transform.rotate = Quaternion::Slerp(animationTime, CalculateValue(fromNodeAnimation.rotate, fromTime), CalculateValue(toNodeAnimation.rotate, toTime));
            joint.transform.scale = Vector3::Lerp(animationTime, CalculateValue(fromNodeAnimation.scale, fromTime), CalculateValue(toNodeAnimation.scale, toTime));
        }
    }
}
