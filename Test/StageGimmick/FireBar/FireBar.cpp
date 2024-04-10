#include "FireBar.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

const std::string Center::kModelName = "block";
const std::string Bar::kModelName = "block";

#pragma region Center
void Center::Initialize(const Desc& desc) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = desc.scale;
	rotate_ = desc.rotate;
	transform.rotate = Quaternion::MakeFromEulerAngle(desc.rotate);
	transform.translate = desc.translate;

	model_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));
	model_->SetIsActive(true);

	onPlayer_ = false;
	onceOnPlayer_ = false;
	isDown_ = false;

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("FireBarCenter");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	collider_->SetSize(transform.scale);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::FireBarCenter);
	collider_->SetCollisionMask(~CollisionAttribute::FireBarCenter);
	collider_->SetIsActive(true);
#pragma endregion
}

void Center::Update() {
	// 一回でもブロックに乗っていて今プレイヤーがブロックに乗っていなかったら
	if (!onPlayer_ &&
		onceOnPlayer_) {
		isDown_ = true;
	}
	// ブロックがおり切ったら
	if (transform.translate.y == (-transform.scale.y * 0.5f) - 1.0f) {
		collider_->SetIsActive(false);
	}
	onPlayer_ = false;
	UpdateTransform();
}

void Center::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	collider_->SetSize(scale);
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Center::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Player") {
		// 落下しているとき
		if (Dot(collisionInfo.normal, Vector3::down) >= 0.8f &&
			player_->GetVelocity().y <= 0.0f) {
			onPlayer_ = true;
			onceOnPlayer_ = true;
		}
	}
}
#pragma endregion

#pragma region Bar
void Bar::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos, float rotateVelocity) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = scale;
	rotate_ = rotate;
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.translate = pos;

	rotateVelocity_ = rotateVelocity;

	model_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));
	model_->SetIsActive(true);

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("FireBarBar");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	collider_->SetSize(transform.scale);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::FireBarBar);
	collider_->SetCollisionMask(~CollisionAttribute::FireBarBar);
	collider_->SetIsActive(true);
#pragma endregion
}

void Bar::Update() {
	rotate_.y += rotateVelocity_;
	rotate_.y = std::fmod(rotate_.y, Math::Pi * 2.0f);
	Vector3 forward = transform.rotate.GetForward();
	transform.translate = pos_ + (forward * transform.scale.z * 0.5f);
	UpdateTransform();
}

void Bar::SetIsActive(bool flag) {
	collider_->SetIsActive(flag);
	model_->SetIsActive(flag);
}

void Bar::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	collider_->SetSize(scale);
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Bar::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
	//if (collisionInfo.collider->GetName() == "Player") {
	//	// 落下しているとき
	//	if (Dot(collisionInfo.normal, Vector3::down) >= 0.8f &&
	//		player_->GetVelocity().y <= 0.0f) {
	//		collider_->SetIsActive(false);
	//		model_->SetIsActive(false);
	//	}
	//}
}

#pragma endregion

void FireBar::Initialize(const Vector3& pos, const Vector3& centerScale, const Vector3& centerRotate, const Vector3& barScale, const Vector3& barRotate, float barRotateVelocity) {
	pos_ = pos;

	center_ = std::make_unique<Center>();
	bar_ = std::make_unique<Bar>();
	center_->SetPlayer(player_);
	bar_->SetPlayer(player_);

	Center::Desc centerDesc{};
	centerDesc.translate = pos_;
	centerDesc.rotate = centerRotate;
	centerDesc.scale = centerScale;
	center_->Initialize(centerDesc);
	bar_->Initialize(barScale, barRotate, pos_, barRotateVelocity);
}

void FireBar::Update() {
	if (center_->GetIsDown()) {
		pos_.y -= 0.05f;
	}
	if (center_->GetOnceOnPlayer()) {
		bar_->SetIsActive(false);
	}
	// -3.0fは床の高さ
	pos_.y = std::max(pos_.y, -center_->GetScale().y - 3.0f);
	center_->SetPosition(pos_);
	bar_->SetPosition(pos_);
	center_->Update();
	bar_->Update();
}
