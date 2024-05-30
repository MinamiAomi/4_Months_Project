#include "PlayerHammer.h"
#include "Framework/ResourceManager.h"
#include "CharacterState.h"

void PlayerHammer::Initialize(const Player* player) {
	player_ = player;
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("hammer"));
	model_->SetIsActive(true);

	transform.SetParent(&player_->transform);
	transform.translate.y = player_->GetModelInstance().GetModel()->GetMeshes()[0].maxVertex.y + 1.0f;
	clampY_ = transform.translate.y;
	particle_ = std::make_unique<PlayerHammerParticle>();
	particle_->Initialize(*this);
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
