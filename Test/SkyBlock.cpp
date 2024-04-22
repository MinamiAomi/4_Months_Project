#include "SkyBlock.h"

#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"


SkyBlock::SkyBlock()
{
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("box"));
	model_->SetIsActive(true);
}

SkyBlock::~SkyBlock()
{
}

void SkyBlock::Initialize(const Vector3& pos, const Vector3& rotate, const Vector3& scale) {
	transform.translate = pos;
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate);
	transform.scale = scale;
	isDead_ = false;
}

void SkyBlock::Update(float apperLength) {
	if (boss_) {
		float distance = std::fabsf(boss_->transform.worldMatrix.GetTranslate().z - transform.translate.z);
		if (distance >= apperLength + 1.0f) {
			isDead_ = true;
		}
	}
	UpdateTransform();
}

void SkyBlock::UpdateTransform() {
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}

