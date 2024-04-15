#include "StageLineLight.h"

#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"

void StageLineLight::Initialize() {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("lightline"));
	model_->SetIsActive(true);
	lightManager_ = &RenderManager::GetInstance()->GetLightManager();

	originTransform_.SetParent(&transform);
	diffTransform_.SetParent(&transform);

	originTransform_.translate.z = model_->GetModel()->GetMeshes()[0].maxVertex.z;
	diffTransform_.translate.z = model_->GetModel()->GetMeshes()[0].minVertex.z;

	lineLight_ = std::make_unique<LineLight>();

	transform.translate.x = 32.0f;
	transform.translate.y = -8.0f;
	transform.scale.z = 300.0f;

}

void StageLineLight::Update() {
	UpdateTransform();
	ImGui::Begin("StageLineLight");
	ImGui::DragFloat3("Color", &lineLight_->color.x, 0.01f, 0.0f);
	ImGui::DragFloat("intensity", &lineLight_->intensity, 0.01f, 0.0f);
	ImGui::DragFloat("decay", &lineLight_->decay, 0.01f, 0.0f);
	ImGui::DragFloat("range", &lineLight_->range, 0.01f, 0.0f);
	ImGui::DragFloat3("StageLineLightTranslate", &transform.translate.x, 0.1f, 0.0f);
	ImGui::DragFloat3("StageLineLightScale", &transform.scale.x, 0.1f, 0.0f);
	ImGui::DragFloat3("StageLineLightRotate", &transform.rotate.x, 0.1f, 0.0f);
	ImGui::End();

	lightManager_->Add(lineLight_);
}

void StageLineLight::UpdateTransform() {
	if (player_) {
		transform.translate.z = player_->GetWorldMatrix().GetTranslate().z;
	}
	transform.UpdateMatrix();
	originTransform_.UpdateMatrix();
	diffTransform_.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
	lineLight_->origin = originTransform_.worldMatrix.GetTranslate();
	lineLight_->diff = diffTransform_.worldMatrix.GetTranslate();


}

