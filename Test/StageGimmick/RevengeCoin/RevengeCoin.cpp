#include "RevengeCoin.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void RevengeCoin::Initialize(const Desc& desc) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = desc.desc.transform.scale;
	transform.rotate = desc.desc.transform.rotate;
	transform.translate = desc.desc.transform.translate;
	transform.UpdateMatrix();
	colliderDesc_ = desc.desc.collider;

	model_->SetModel(ResourceManager::GetInstance()->FindModel(desc.desc.name));
	model_->SetIsActive(true);
	model_->SetWorldMatrix(transform.worldMatrix);
	isAlive_ = true;

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("RevengeCoin");
	collider_->SetCenter(colliderDesc_->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * colliderDesc_->rotate);
	collider_->SetSize({ transform.scale.x * colliderDesc_->size.x ,transform.scale.y * colliderDesc_->size.y ,transform.scale.z * colliderDesc_->size.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::RevengeCoin);
	collider_->SetCollisionMask(~CollisionAttribute::RevengeCoin);
	collider_->SetIsActive(true);
#pragma endregion
	UpdateTransform();
}

void RevengeCoin::Update() {
	if (std::fabs((camera_->GetPosition() - transform.worldMatrix.GetTranslate()).Length()) <= 200.0f) {

		UpdateTransform();
		// 雑カリング
		model_->SetIsActive(true);
		collider_->SetIsActive(true);

		if (!isAlive_) {
			model_->SetIsActive(false);
			collider_->SetIsActive(false);
		}
	}
	else {
		model_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
}

void RevengeCoin::Reset() {
	isAlive_ = false;
	model_->SetIsActive(true);
	collider_->SetIsActive(true);
}

void RevengeCoin::UpdateTransform() {
	transform.UpdateMatrix();
	collider_->SetCenter(colliderDesc_->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * colliderDesc_->rotate);
	collider_->SetSize({ transform.scale.x * colliderDesc_->size.x ,transform.scale.y * colliderDesc_->size.y ,transform.scale.z * colliderDesc_->size.z });
	model_->SetWorldMatrix(transform.worldMatrix);
}

void RevengeCoin::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Player") {
		isAlive_ = false;
	}
}
