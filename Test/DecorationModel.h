#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Math/MathUtils.h"
#include "Graphics/Model.h"
#include "Player/Player.h"

class DecorationModel :
	public GameObject {
public:
	void Initialize(std::string modelName);
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
private:
	void UpdateTransform();

	const Player* player_;

	std::unique_ptr<ModelInstance> model_;
};