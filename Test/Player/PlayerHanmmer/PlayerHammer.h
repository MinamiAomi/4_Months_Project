#pragma once

#include <cstdint>
#include <array>
#include "Graphics/Model.h"
#include "Math/Transform.h"
#include "Math/Random.h"
#include "Collision/GameObject.h"
class Player;
class PlayerHammer :
	public GameObject {
public:
	

	void Initialize(const Player* player);
	void Update();
	void UpdateTransform();

	void SetPlayer(Player* player) { player_ = player; }
	

private:

	const Player* player_;
	std::unique_ptr<ModelInstance> model_;
	
};