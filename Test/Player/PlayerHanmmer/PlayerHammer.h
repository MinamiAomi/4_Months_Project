#pragma once

#include <cstdint>
#include <array>
#include "Graphics/Model.h"
#include "Math/Transform.h"
#include "Math/Random.h"
#include "Collision/GameObject.h"
#include "PlayerHammerParticle.h"
#include "Player/Player.h"
#include "Graphics/LightManager.h"
class PlayerHammer :
	public GameObject {
public:
	

	void Initialize(const Player* player);
	void Update();
	void UpdateTransform();

	void SetPlayer(Player* player) { player_ = player; }
	
	void SetIsActive();

	void SetIsInactive();

	ModelInstance& GetModel() { return *model_.get(); }

	const Player* GetPlayer() { return player_; }

private:

	const Player* player_;
	LightManager* lightManager_ = nullptr;
	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<PlayerHammerParticle> particle_;
	float clampY_;
	float velocity_;
	float acceleration_;

	std::shared_ptr<PointLight> pointLight_;
	Transform pointLightTransform_;
};