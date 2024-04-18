#include "StageBlock.h"

#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"


StageBlock::StageBlock()
{
}

StageBlock::~StageBlock()
{
}

void StageBlock::Initialize(const Vector3& pos, const Vector3& direction) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("box"));
	model_->SetIsActive(true);
	transform.scale = { 2.0f,36.0f,2.0f };
	direction_ = direction;
	transform.translate = pos;
	transform.rotate = Quaternion::MakeFromTwoVector({0.0f,1.0f,0.0f},direction);
	appearFrame_ = kAppearFrame;
	isDead_ = false;
	initialState_ = characterState_;
}

void StageBlock::Update() {
	appearFrame_--;
	if (appearFrame_ >= 0) {
		if (static_cast<int>(transform.translate.z) % 2 == 0) {
			transform.rotate = Quaternion::MakeFromAngleAxis(5.0f * Math::ToRadian, direction_) * transform.rotate;
			transform.translate += direction_ * 0.6f;
		}
		else {
			transform.rotate = Quaternion::MakeFromAngleAxis(-4.0f * Math::ToRadian, direction_) * transform.rotate;
			transform.translate += direction_ * 0.6f;
		}

	}
	if (boss_) {
		float distance = (boss_->transform.worldMatrix.GetTranslate() - transform.translate).Length();
		if (distance >= 150.0f) {
			isDead_ = true;
		}
	}
	if (initialState_ != characterState_) {
		isDead_ = true;
	}
	UpdateTransform();
}

void StageBlock::UpdateTransform() {
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}

