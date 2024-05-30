#include "Trap.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Culling.h"
#include "Engine/Math/MathUtils.h"
#include "Graphics/ImGuiManager.h"
#include "Framework/ResourceManager.h"
#include "Player/Player.h"
#include "Math/MathUtils.h"
#include "Boss/Boss.h"


void Trap::Initialize(const Desc& desc) {
	model_ = std::make_unique<ModelInstance>();

	desc_ = desc;
	transform.scale = Vector3::one;
	transform.rotate = Quaternion::identity;
	transform.translate = desc.pos;
	transform.translate.y = desc_.offset;

	isAlive_ = true;

	model_->SetModel(ResourceManager::GetInstance()->FindModel("block"));
	model_->SetIsActive(true);

	state_ = State::kDrop;

	time_ = 0.0f;

	random_.x = rnd_.NextFloatRange(-30.0f, 30.0f);
	random_.z = rnd_.NextFloatRange(-5.0f, 5.0f);
	random_.y = rnd_.NextFloatRange(30.0f, 40.0f);

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Trap");
	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ transform.scale.x * modelSize.x ,transform.scale.y * modelSize.y ,transform.scale.z * modelSize.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::DropGimmickDropperBall);
	collider_->SetCollisionMask(CollisionAttribute::Player | CollisionAttribute::Ground);
	collider_->SetIsActive(true);
#pragma endregion
	UpdateTransform();
}

void Trap::Update() {
	// カリングしてない（めんどくさいだけ）

	switch (state_) {
	case Trap::kDrop:
		transform.translate = { player_->transform.worldMatrix.GetTranslate().x,transform.translate.y,player_->transform.worldMatrix.GetTranslate().z };
		transform.translate.y -= desc_.dropVelocity;
		// 地面を貫通したときに
		if (transform.translate.y <= -4.0f ||
			Character::currentCharacterState_ == Character::State::kChase) {
			isAlive_ = false;
		}
		break;
	case Trap::kStay:

		break;
	case Trap::kShot:
		static const float kMax = 60.0f;

		transform.translate = Vector3::QuadraticBezierCurve(
			time_ / kMax,
			setPos_,
			setPos_ + random_ + Vector3(0.0f, 0.0f, boss_->transform.worldMatrix.GetTranslate().z * 0.5f),
			boss_->transform.worldMatrix.GetTranslate());
		time_++;
		break;
	}
	UpdateTransform();
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
			state_ == State::kStay) {
			state_ = State::kShot;
		}
	}
	else if (collisionInfo.collider->GetName() == "Boss") {
		if (Character::currentCharacterState_ == Character::State::kChase &&
			state_ == State::kShot) {
			isAlive_ = false;
		}
	}
	else if (collisionInfo.collider->GetName() == "Block" ||
		collisionInfo.collider->GetName() == "FireBarCenter" ||
		collisionInfo.collider->GetName() == "Floor" ||
		collisionInfo.collider->GetName() == "StageObject") {
		if (state_ == State::kDrop) {

			state_ = State::kStay;
			setPos_ = transform.worldMatrix.GetTranslate();
		}
	}
}
