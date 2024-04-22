#include "DecorationModel.h"

#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void DecorationModel::Initialize(std::string modelName) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel(modelName));
	model_->SetIsActive(true);
}

void DecorationModel::Update() {
	UpdateTransform();
}

void DecorationModel::UpdateTransform() {
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}

