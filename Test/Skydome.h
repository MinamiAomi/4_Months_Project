#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Math/MathUtils.h"
#include "Graphics/Model.h"

class Player;

class Skydome :
	public GameObject {
public:
	void Initialize(Player* player);
	void Update();
public:

private:
	std::unique_ptr<ModelInstance> model_;
	Player* player_;
};