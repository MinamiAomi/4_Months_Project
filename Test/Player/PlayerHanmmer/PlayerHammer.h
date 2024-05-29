#pragma once

#include <cstdint>
#include <array>
#include "Graphics/Model.h"
#include "Math/Transform.h"
#include "Math/Random.h"
#include "Collision/GameObject.h"
#include "PlayerHammerParticle.h"
#include "Player/Player.h"
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
	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<PlayerHammerParticle> particle_;
	
};