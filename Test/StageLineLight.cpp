#include "StageLineLight.h"

#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"

void StageLineLight::Initialize(bool isLeft) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("lightline"));
	model_->SetIsActive(true);
	lightManager_ = &RenderManager::GetInstance()->GetLightManager();

	originTransform_.SetParent(&transform);
	diffTransform_.SetParent(&transform);

	originTransform_.translate.z = model_->GetModel()->GetMeshes()[0].maxVertex.z;
	diffTransform_.translate.z = model_->GetModel()->GetMeshes()[0].minVertex.z;

	lineLight_ = std::make_unique<LineLight>();

	if (isLeft) {
		transform.translate.x = -32.0f;
	}
	else {
		transform.translate.x = 32.0f;
	}
	transform.translate.y = -23.0f;
	transform.scale.z = 300.0f;

	lineLight_->color = { 0.9f,0.0f,0.60f };
	lineLight_->intensity = 1.5f;
	lineLight_->decay = 0.85f;
	lineLight_->range = 15.0f;

}

void StageLineLight::Update() {
	UpdateTransform();
#ifdef _DEBUG
	ImGui::Begin("StageLineLight");
	ImGui::DragFloat3("Color", &lineLight_->color.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("intensity", &lineLight_->intensity, 0.01f, 0.1f);
	ImGui::DragFloat("decay", &lineLight_->decay, 0.01f, 0.0f);
	ImGui::DragFloat("range", &lineLight_->range, 0.01f, 0.0f);
	ImGui::DragFloat3("StageLineLightTranslate", &transform.translate.x, 0.1f, 0.0f);
	ImGui::DragFloat3("StageLineLightScale", &transform.scale.x, 0.1f, 0.0f);
	ImGui::DragFloat3("StageLineLightRotate", &transform.rotate.x, 0.1f, 0.0f);
	ImGui::DragFloat3("LineLightTranslate", &originTransform_.translate.x, 0.1f, 0.0f);
	ImGui::End();
#endif // _DEBUG

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
	lineLight_->diff.z = diffTransform_.worldMatrix.GetTranslate().z;
}

