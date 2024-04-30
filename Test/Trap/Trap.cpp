#include "Trap.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Culling.h"
#include "Engine/Math/MathUtils.h"
#include "Graphics/ImGuiManager.h"
#include "Framework/ResourceManager.h"
#include "Player/Player.h"

void Trap::Initialize(const Desc& desc) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = Vector3::one;
	transform.rotate = Quaternion::identity;
	transform.translate = desc.pos;
	transform.UpdateMatrix();

	velocity_ = desc.velocity;

	isMove_ = false;
	isAlive_ = true;

	model_->SetModel(ResourceManager::GetInstance()->FindModel("block"));
	model_->SetIsActive(true);
	model_->SetWorldMatrix(transform.worldMatrix);
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Trap");
	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ transform.scale.x * modelSize.x ,transform.scale.y * modelSize.y ,transform.scale.z * modelSize.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Trap);
	collider_->SetCollisionMask(~CollisionAttribute::Trap);
	collider_->SetIsActive(true);
#pragma endregion
}

void Trap::Update() {
	// カリングしてない（めんどくさいだけ）
	if (isAlive_) {
		collider_->SetIsActive(true);
		model_->SetIsActive(true);
		transform.translate.z += velocity_;
		UpdateTransform();
	}
	else {
		collider_->SetIsActive(false);
		model_->SetIsActive(false);
	}
	/*if (Culling::Culling(transform, *model_.get(), *camera_) && isMove_) {
	}
	else {
		collider_->SetIsActive(false);
		model_->SetIsActive(false);
	}*/
}

void Trap::UpdateTransform() {
	transform.UpdateMatrix();
	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ transform.scale.x * modelSize.x ,transform.scale.y * modelSize.y ,transform.scale.z * modelSize.z });
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Trap::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Player") {
		if (Character::currentCharacterState_ == Character::State::kChase &&
			(Dot(collisionInfo.normal, Vector3::down) >= 0.8f &&
				player_->GetVelocity().y <= 0.0f)) {
			isMove_ = true;
		}
	}
	else if (collisionInfo.collider->GetName() == "Boss") {
		if (Character::currentCharacterState_ == Character::State::kChase)
			isAlive_ = false;
	}
}
