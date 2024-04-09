#include "floor.h"

#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void Floor::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("floor"));
	model_->SetIsActive(true);

	rotate_ = rotate;

	transform.translate = pos;
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.scale = scale;


#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Floor");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	collider_->SetSize({ transform.scale.x * 8.0f,transform.scale.y,transform.scale.z * 40.0f });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Floor);
	collider_->SetCollisionMask(~CollisionAttribute::Floor);
	collider_->SetIsActive(true);
#pragma endregion
}

void Floor::Update() {
	UpdateTransform();
}

void Floor::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	collider_->SetSize({ scale.x * 8.0f,scale.y,scale.z * 40.0f });
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Floor::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}
