#include "PlayerHammer.h"
#include "Player/Player.h"
#include "Framework/ResourceManager.h"
#include "Player/Player.h"

void PlayerHammer::Initialize(const Player* player) {
	player_ = player;
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("hammer"));
	model_->SetIsActive(true);

	transform.SetParent(&player_->transform);
	transform.translate.y = player_->GetModelInstance().GetModel()->GetMeshes()[0].maxVertex.y;

}

void PlayerHammer::Update() {
	
}

void PlayerHammer::UpdateTransform()
{
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}
