#include "StageBlock.h"

#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"

void StageBlock::Initialize() {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("box"));
	model_->SetIsActive(true);
}

void StageBlock::Update() {
	UpdateTransform();
}

void StageBlock::UpdateTransform() {
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}

