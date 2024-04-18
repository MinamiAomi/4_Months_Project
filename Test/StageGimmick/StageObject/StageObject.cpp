#include "StageObject.h"

#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void StageObject::Initialize(const StageGimmick::Desc& desc) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = desc.transform.scale;
	rotate_ = desc.transform.rotate;
	transform.translate = desc.transform.translate;

	colliderDesc_ = desc.collider;

	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);

	model_->SetModel(ResourceManager::GetInstance()->FindModel(desc.name));
	model_->SetIsActive(true);

#pragma region コライダー
	if (colliderDesc_) {
		collider_ = std::make_unique<BoxCollider>();
		collider_->SetGameObject(this);
		collider_->SetName("StageObject");
		collider_->SetCenter(colliderDesc_->center * transform.worldMatrix);
		collider_->SetOrientation(transform.rotate * Quaternion::MakeFromEulerAngle(colliderDesc_->rotate));
		collider_->SetSize(colliderDesc_->size);
		//collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
		collider_->SetCollisionAttribute(CollisionAttribute::StageObject);
		collider_->SetCollisionMask(~CollisionAttribute::StageObject);
		collider_->SetIsActive(true);
	}
#pragma endregion
}

void StageObject::Update() {
	UpdateTransform();
}

void StageObject::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.UpdateMatrix();
	if (colliderDesc_) {
		collider_->SetCenter(colliderDesc_->center * transform.worldMatrix);
		collider_->SetOrientation(transform.rotate * Quaternion::MakeFromEulerAngle(colliderDesc_->rotate));
		collider_->SetSize(colliderDesc_->size);
	}
	model_->SetWorldMatrix(transform.worldMatrix);
}

//void StageObject::OnCollision(const CollisionInfo& collisionInfo) {}
