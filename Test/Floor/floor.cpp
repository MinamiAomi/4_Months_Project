#include "floor.h"

#include "Graphics/ResourceManager.h"

void Floor::Initialize() {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("floor"));
	model_->SetIsActive(true);

	transform.translate = Vector3::zero;
	transform.rotate = Quaternion::identity;
	transform.scale = Vector3::one;

	zLength_ = model_->GetModel()->GetMeshes()[0].maxVertex.z - model_->GetModel()->GetMeshes()[0].minVertex.z;
}

void Floor::Update() {
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}
