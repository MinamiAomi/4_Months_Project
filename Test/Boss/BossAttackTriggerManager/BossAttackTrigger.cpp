#include "BossAttackTrigger.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ImGuiManager.h"
#include "File/JsonHelper.h"
#include "Graphics/ResourceManager.h"

void BossAttackTrigger::Initialize(const Desc desc) {
	Reset();
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("box"));
	model_->SetIsActive(true);

	desc_ = desc;

	transform.translate.z = desc_.pos;
	transform.scale = { 10.0f,1.0f,1.0f };

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("BossAttackTrigger");
	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::BossAttackTrigger);
	collider_->SetCollisionMask(~CollisionAttribute::BossAttackTrigger);
	collider_->SetIsActive(true);
#pragma endregion
}

void BossAttackTrigger::Update() {
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	collider_->SetSize(scale);
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void BossAttackTrigger::Reset() {
	isCollision_ = false;
}

void BossAttackTrigger::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Boss"
		&& characterState_ == Character::kRunAway
		&& !isCollision_) {
		isCollision_ = true;
		switch (desc_.state) {
		case BossStateManager::kRoot:
			boss_->GetStateManager()->ChangeState<BossStateRoot>();
			break;
		case BossStateManager::kAttack:
			boss_->GetStateManager()->ChangeState<BossStateAttack>();
			break;
		default:
			break;
		}
	}
}
