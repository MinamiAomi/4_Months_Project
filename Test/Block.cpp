#include "Block.h"

#include "Graphics/ResourceManager.h"

#include "Graphics/ImGuiManager.h"

const std::string Block::kModelName = "Block";

void Block::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos) {
	model_ = std::make_unique<ModelInstance>();
	
	transform.scale = scale;
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate);
	transform.translate = pos;

	model_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));
	model_->SetIsActive(true);
}

void Block::Update() {
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Block::Draw() {
}
