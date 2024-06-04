#include "PlayerHammer.h"
#include "Framework/ResourceManager.h"
#include "CharacterState.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"

void PlayerHammer::Initialize(const Player* player) {
	player_ = player;
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("hammer"));
	model_->SetIsActive(true);
	model_->SetWireFrameMask(true);

	transform.SetParent(&player_->transform);
	transform.translate.y = player_->GetModelInstance().GetModel()->GetMeshes()[0].maxVertex.y + 1.0f;
	clampY_ = transform.translate.y;
	particle_ = std::make_unique<PlayerHammerParticle>();
	particle_->Initialize(*this);

	lightManager_ = &RenderManager::GetInstance()->GetLightManager();
	pointLight_ = std::make_shared<PointLight>();
	pointLight_->color = { 1.0f,1.0f,0.0f };
	pointLight_->decay = 2.8f;
	pointLight_->intensity = 8.5f;
	pointLight_->range = 9.2f;

	pointLightTransform_.SetParent(&transform, false);
	pointLightTransform_.translate = { 0.0f,4.0f,0.0f };
}

void PlayerHammer::Update() {

	
	particle_->Update();
	transform.translate.y += player_->GetVelocity().y / 3.0f;
	transform.translate.y = std::clamp(transform.translate.y, clampY_, FLT_MAX);

	if (!player_->GetIsAlive() && model_->IsActive()) {
		SetIsInactive();
	}
	if (Character::IsInSceneChange()) {
		if (Character::preCharacterState_ == Character::kChase) {
			SetIsInactive();
		}
		if (Character::preCharacterState_ == Character::kRunAway) {
			SetIsActive();
		}
	}
	/*ImGui::Begin("han");
	ImGui::DragFloat3("LightsPos", &pointLightTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("Color", &pointLight_->color.x, 0.01f);
	ImGui::DragFloat("decay", &pointLight_->decay, 0.01f);
	ImGui::DragFloat("intensity", &pointLight_->intensity, 0.01f);
	ImGui::DragFloat("range", &pointLight_->range, 0.01f);
	ImGui::End();*/

	pointLightTransform_.UpdateMatrix();
	pointLight_->position = pointLightTransform_.worldMatrix.GetTranslate();
	if (model_->IsActive()) {
		lightManager_->Add(pointLight_);
	}
	UpdateTransform();
}

void PlayerHammer::UpdateTransform()
{
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}

void PlayerHammer::SetIsActive()
{
	particle_->SetFrame(10,true);
}

void PlayerHammer::SetIsInactive()
{
	particle_->SetFrame(10, false);
}
