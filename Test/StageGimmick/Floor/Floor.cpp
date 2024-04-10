#include "floor.h"

#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void Floor::Initialize(const Desc& desc) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("floor"));
	model_->SetIsActive(true);

	rotate_ = desc.rotate;

	transform.translate = desc.translate;
	transform.rotate = Quaternion::MakeFromEulerAngle(desc.rotate);
	transform.scale = desc.scale;

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Floor");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
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
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Floor::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}
