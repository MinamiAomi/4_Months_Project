#include "floor.h"

#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void Floor::Initialize(const Desc& desc) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = desc.desc.transform.scale;
	transform.rotate = desc.desc.transform.rotate;
	transform.translate = desc.desc.transform.translate;

	colliderDesc_ = desc.desc.collider;

	model_->SetModel(ResourceManager::GetInstance()->FindModel(desc.desc.name));
	model_->SetIsActive(true);

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Floor");
	collider_->SetCenter(colliderDesc_->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * colliderDesc_->rotate);
	collider_->SetSize({ transform.scale.x * colliderDesc_->size.x ,transform.scale.y * colliderDesc_->size.y ,transform.scale.z * colliderDesc_->size.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Floor);
	collider_->SetCollisionMask(~CollisionAttribute::Floor);
	collider_->SetIsActive(true);
#pragma endregion
	UpdateTransform();
}

void Floor::Update() {
	// 雑カリング
	// こいつだけいっぱいのバス
	if (std::fabs((camera_->GetPosition() - transform.worldMatrix.GetTranslate()).Length()) <= 200.0f) {
		model_->SetIsActive(true);
		collider_->SetIsActive(true);
		UpdateTransform();
	}
	else {
		model_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
}

void Floor::UpdateTransform() {
	transform.UpdateMatrix();
	collider_->SetCenter(colliderDesc_->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * colliderDesc_->rotate);
	collider_->SetSize({ transform.scale.x * colliderDesc_->size.x ,transform.scale.y * colliderDesc_->size.y ,transform.scale.z * colliderDesc_->size.z });
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Floor::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}
