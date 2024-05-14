#include "BeltConveyor.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void BeltConveyor::Initialize(const Desc& desc) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = desc.desc.transform.scale;
	transform.rotate = desc.desc.transform.rotate;
	transform.translate = desc.desc.transform.translate;

	desc_ = desc;
	colliderDesc_ = desc.desc.collider;

	model_->SetModel(ResourceManager::GetInstance()->FindModel(desc.desc.name));
	model_->SetIsActive(true);

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("BeltConveyor");
	collider_->SetCenter(colliderDesc_->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * colliderDesc_->rotate);
	collider_->SetSize({ transform.scale.x * colliderDesc_->size.x ,transform.scale.y * colliderDesc_->size.y ,transform.scale.z * colliderDesc_->size.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Block);
	collider_->SetCollisionMask(~CollisionAttribute::Block);
	collider_->SetIsActive(true);
#pragma endregion
	UpdateTransform();
}

void BeltConveyor::Update() {
	if (std::fabs((camera_->GetPosition() - transform.worldMatrix.GetTranslate()).Length()) <= 200.0f) {
		transform.translate.y = (std::max)(transform.translate.y, (-transform.scale.y * 0.5f) - 3.0f);
		UpdateTransform();
		// 雑カリング
		model_->SetIsActive(true);
		collider_->SetIsActive(true);
	}
	else {
		model_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
}

void BeltConveyor::UpdateTransform() {
	transform.UpdateMatrix();
	collider_->SetCenter(colliderDesc_->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * colliderDesc_->rotate);
	collider_->SetSize({ transform.scale.x * colliderDesc_->size.x ,transform.scale.y * colliderDesc_->size.y ,transform.scale.z * colliderDesc_->size.z });
	model_->SetWorldMatrix(transform.worldMatrix);
}

void BeltConveyor::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Player") {
		// 落下しているとき
		if (Dot(collisionInfo.normal, Vector3::down) >= 0.8f &&
			player_->GetVelocity().y <= 0.0f) {
			player_->SetBeltConveyorVelocity(desc_.velocity);
		}
	}
}
