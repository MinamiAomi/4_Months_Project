#include "Skydome.h"

#include "Graphics/ResourceManager.h"
#include "Player/Player.h"

void Skydome::Initialize(const Player* player) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("skydome"));
	model_->SetIsActive(true);
	//model_->SetUseLighting(false);

	transform.translate = Vector3::zero;
	transform.rotate = Quaternion::identity;
	transform.scale = {200.0f,200.0f,200.0f};

	player_ = player;
}

void Skydome::Update() {
	
	transform.translate.z = player_->GetWorldMatrix().GetTranslate().z;
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}
