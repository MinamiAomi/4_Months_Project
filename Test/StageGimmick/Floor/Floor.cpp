#include "floor.h"

#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void Floor::Initialize(const StageGimmick::Desc& desc) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = desc.transform.scale;
	rotate_ = desc.transform.rotate;
	transform.translate = desc.transform.translate;

	colliderDesc_ = desc.collider;

	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);

	model_->SetModel(ResourceManager::GetInstance()->FindModel(desc.name));
	model_->SetIsActive(true);

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Floor");
	collider_->SetCenter(colliderDesc_.center);
	collider_->SetOrientation(Quaternion::MakeFromEulerAngle(colliderDesc_.rotate));
	//Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	//collider_->SetSize({ modelSize.x * (colliderDesc_.size.x * 0.5f) ,modelSize.y * (colliderDesc_.size.y * 0.5f) ,modelSize.z * (colliderDesc_.size.z * 0.5f) });
	collider_->SetSize(colliderDesc_.size);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Block);
	collider_->SetCollisionMask(~CollisionAttribute::Block);
	collider_->SetIsActive(true);
#pragma endregion
}

void Floor::Update() {
	// 雑カリング
	// こいつだけいっぱいのバス
	if (std::fabs((camera_ ->GetPosition() - transform.worldMatrix.GetTranslate()).Length()) <= 200.0f) {
		model_->SetIsActive(true);
		collider_->SetIsActive(true);
	}
	else {
		model_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
	UpdateTransform();
}

void Floor::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.UpdateMatrix();
	collider_->SetCenter(colliderDesc_.center);
	collider_->SetOrientation(Quaternion::MakeFromEulerAngle(colliderDesc_.rotate));
	collider_->SetSize(colliderDesc_.size); 
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Floor::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}
