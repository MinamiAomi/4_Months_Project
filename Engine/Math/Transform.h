#pragma once

#include "MathUtils.h"

class Transform {
public:
    void UpdateMatrix() {
        worldMatrix = Matrix4x4::MakeAffineTransform(scale, rotate, translate);
        if (parent_) {
            if (!isParentRotate) {
                worldMatrix *= Matrix4x4::MakeRotation(parent_->rotate).Inverse();
            }
            worldMatrix *= parent_->worldMatrix;
        }
    }

    void SetParent(const Transform* parent, bool applyParentSpace = true) {
        if (applyParentSpace) {
            // 元々親がいた場合一度ワールド空間に戻す
            if (parent_) {
                scale = worldMatrix.GetScale();
                rotate = worldMatrix.GetRotate();
                translate = worldMatrix.GetTranslate();
            }
            parent_ = parent;
            // 新しい親がいる場合親空間のローカルにする
            if (parent_) {
                Matrix4x4 localMatrix = worldMatrix * parent_->worldMatrix.Inverse();
                scale = localMatrix.GetScale();
                rotate = localMatrix.GetRotate();
                translate = localMatrix.GetTranslate();
                return;
            }
        }
        else {
            parent_ = parent;
        }
        
    }

    const Transform* GetParent() const { return parent_; }

    Vector3 scale = Vector3::one;
    Quaternion rotate;
    Vector3 translate;
    Matrix4x4 worldMatrix;
    bool isParentRotate = true;

private:
    const Transform* parent_ = nullptr;
};