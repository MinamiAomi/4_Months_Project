#include "PlayerBullet.h"

#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"

void PlayerBullet::Initialize() {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("box"));
	model_->SetIsActive(false);

	transform.translate = Vector3::zero;
	transform.rotate = Quaternion::identity;
	transform.scale = { 2.0f,2.0f,2.0f };

	isAlive_ = false;
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Bullet");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	collider_->SetSize(transform.scale);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::PlayerBullet);
	collider_->SetCollisionMask(~CollisionAttribute::Player);
	collider_->SetIsActive(false);
#pragma endregion
}

void PlayerBullet::Update() {
	if (isAlive_) {
		transform.translate += velocity_;
		UpdateTransform();
	}
}

void PlayerBullet::Create(const Vector3& pos, const Vector3& velocity) {
	transform.translate = { pos.x,pos.y + transform.scale.y,pos.z };
	transform.rotate = Quaternion::identity;
	velocity_ = velocity;
	model_->SetIsActive(true);
	collider_->SetIsActive(true);
	isAlive_ = true;
}

void PlayerBullet::Reset() {
	isAlive_ = false;
	model_->SetIsActive(false);
	collider_->SetIsActive(false);
}

void PlayerBullet::UpdateTransform() {
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	collider_->SetSize(scale);
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void PlayerBullet::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Boss" ||
		collisionInfo.collider->GetName() == "Block") {
		isAlive_ = false;
		model_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
}
