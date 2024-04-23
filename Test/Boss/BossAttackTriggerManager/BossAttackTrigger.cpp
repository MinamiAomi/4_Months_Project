#include "BossAttackTrigger.h"

#include "Boss/Boss.h"
#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ImGuiManager.h"
#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"

void BossAttackTrigger::Initialize(const Desc desc) {
	Reset();
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("box"));
	model_->SetIsActive(true);

	desc_ = desc;

	transform.translate.z = desc_.pos;
	transform.scale = { 20.0f,20.0f,1.0f };

#ifdef _DEBUG
	model_->SetIsActive(true);
#else
	model_->SetIsActive(false);
#endif // _DEBUG

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
	if (std::fabs((camera_->GetPosition() - transform.worldMatrix.GetTranslate()).Length()) <= 200.0f) {
		//model_->SetIsActive(true);
		collider_->SetIsActive(true);
	}
	else {
		//model_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
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

void BossAttackTrigger::SetDesc(const Desc& desc) {
	desc_ = desc;

	transform.translate.z = desc_.pos;
	transform.UpdateMatrix();
}

void BossAttackTrigger::SetIsAlive(bool flag) {
	collider_->SetIsActive(flag);
	model_->SetIsActive(flag);
}

void BossAttackTrigger::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Boss" &&
		boss_->GetStateManager()->GetState() == BossStateManager::State::kRoot &&
		Character::currentCharacterState_ == Character::kRunAway &&
		!isCollision_) {
		isCollision_ = true;
		switch (desc_.state) {
		case BossStateManager::kRoot:
			boss_->GetStateManager()->ChangeState<BossStateRoot>();
			break;
		case BossStateManager::kHook:
			boss_->GetStateManager()->ChangeState<BossStateHook>();
			break;
		case BossStateManager::kFloorAll:
			boss_->GetStateManager()->ChangeState<BossStateFloorAll>();
			break;
		case BossStateManager::kLongDistanceAttack:
			boss_->GetStateManager()->ChangeState<BossStateLongDistanceAttack>();
			break;
		default:
			break;
		}
	}
}
