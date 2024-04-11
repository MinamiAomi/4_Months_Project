#include "BossModelManager.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"

namespace BossParts {
	// 実際の定義
	std::array<std::string, Parts::kCount> partsName_ = {
		"bossBody",
		"bossRightArm",
		"bossLeftArm",
	};
}

void BossModelManager::Initialize(const Transform* Transform) {
	models_.at(BossParts::Parts::kBody) = std::make_unique<Body>();
	models_.at(BossParts::Parts::kRightArm) = std::make_unique<RightArm>();
	models_.at(BossParts::Parts::kLeftArm) = std::make_unique<LeftArm>();

	for (uint32_t i = 0; auto & model : models_) {
		model->transform.SetParent(Transform);
		model->Initialize(i);
		i++;
	}
}

void BossModelManager::Update() {
	for (auto& model : models_) {
		model->Update();
	}
}

void BossModel::Initialize(uint32_t index) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel(BossParts::partsName_.at(index)));
	model_->SetIsActive(true);

	transform.UpdateMatrix();
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName(BossParts::partsName_.at(index));
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Boss);
	collider_->SetCollisionMask(~CollisionAttribute::Boss);
	collider_->SetIsActive(true);
#pragma endregion
}

void BossModel::Update() {
	UpdateTransform();
}

void BossModel::UpdateTransform() {
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

void Body::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void RightArm::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void LeftArm::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}