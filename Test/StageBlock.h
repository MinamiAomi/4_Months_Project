#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Math/MathUtils.h"
#include "Graphics/Model.h"
#include "Player/Player.h"

#include "Graphics/LightManager.h"

class StageBlock :
	public GameObject {
public:
	void Initialize();
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
private:
	void UpdateTransform();

	const Player* player_;

	std::unique_ptr<ModelInstance> model_;
};