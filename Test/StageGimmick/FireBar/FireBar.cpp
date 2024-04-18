#include "FireBar.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

const std::string Bar::kModelName = "fireBarBar";

#pragma region Bar
void Bar::Initialize(const Desc& desc) {
	rotateVelocity_ = desc.rotateVelocity;
	model_ = std::make_unique<ModelInstance>();

	transform.scale = Vector3::one;
	transform.scale.x = desc.length;

	rotate_ = desc.barInitialAngle;
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);

	model_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));
	model_->SetIsActive(true);

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("FireBarBar");
	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	collider_->SetOrientation(transform.rotate);

	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::FireBarBar);
	collider_->SetCollisionMask(~CollisionAttribute::FireBarBar);
	collider_->SetIsActive(true);
#pragma endregion
}

void Bar::Update() {
	rotate_.y += rotateVelocity_;
	rotate_.y = std::fmod(rotate_.y, Math::Pi * 2.0f);
	UpdateTransform();
}

void Bar::SetDesc(const Desc& desc) {
	rotateVelocity_ = desc.rotateVelocity;

	transform.scale.x = desc.length;

	rotate_ = desc.barInitialAngle;
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
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Bar::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

#pragma endregion

void FireBar::Initialize(const Desc& desc) {
	desc_ = desc;

	transform = desc.transform;

	model_ = std::make_unique<ModelInstance>();

	model_->SetModel(ResourceManager::GetInstance()->FindModel("fireBarCenter"));
	model_->SetIsActive(true);

	onPlayer_ = false;
	onceOnPlayer_ = false;
	isDown_ = false;

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("FireBarCenter");
	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	collider_->SetOrientation(transform.rotate);

	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::FireBarCenter);
	collider_->SetCollisionMask(~CollisionAttribute::FireBarCenter);
	collider_->SetIsActive(true);
#pragma endregion

	bar_ = std::make_unique<Bar>();

	bar_->transform.SetParent(&transform);
	bar_->SetPlayer(player_);

	bar_->Initialize(desc.barDesc);
}

void FireBar::Update() {

	// 一回でもブロックに乗っていて今プレイヤーがブロックに乗っていなかったら
	if (!onPlayer_ &&
		onceOnPlayer_) {
		isDown_ = true;
	}
	onPlayer_ = false;
	// ブロックが下がる
	if (isDown_) {
		transform.translate.y -= 0.05f;
	}
	// ブロックがおり切ったら
	if (transform.translate.y <= (-transform.scale.y * 0.5f) - 1.0f) {
		collider_->SetIsActive(false);
		model_->SetIsActive(false);
	}
	// 一度でもプレイヤーがブロックのうえに乗ったら
	if (onceOnPlayer_) {
		bar_->SetIsActive(false);
	}
	// 雑カリング
	if (std::fabs((camera_->GetPosition() - transform.worldMatrix.GetTranslate()).Length()) <= 200.0f) {
		model_->SetIsActive(true);
		collider_->SetIsActive(true);
	}
	else {
		model_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
	UpdateTransform();
	bar_->Update();
}

void FireBar::SetDesc(const Desc& desc) {
	desc_ = desc;
	transform = desc.transform;

	bar_->SetDesc(desc.barDesc);
}

void FireBar::SetIsActive(bool flag) {
	collider_->SetIsActive(flag);
	model_->SetIsActive(flag);
	bar_->SetIsActive(flag);
}

void FireBar::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Player") {
		// 落下しているとき
		if (Dot(collisionInfo.normal, Vector3::down) >= 0.8f &&
			player_->GetVelocity().y <= 0.0f) {
			onPlayer_ = true;
			onceOnPlayer_ = true;
		}
	}
}

void FireBar::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);

	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}
