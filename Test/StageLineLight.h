#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Math/MathUtils.h"
#include "Graphics/Model.h"
#include "Player/Player.h"

#include "Graphics/LightManager.h"

#include "../Test/CharacterState.h"

class StageLineLight :
	public GameObject {
public:
	void Initialize(bool isLeft);
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
private:
	void UpdateTransform();

	const Player* player_;

	std::unique_ptr<ModelInstance> model_;
	
	LightManager* lightManager_;
	std::shared_ptr<LineLight> lineLight_;
	Transform originTransform_;
	Transform diffTransform_;

	Character::State saveState_;
	float t_ = 0.0f;
	float speed_ = 0.01f;

	Vector3 runAwayColor_ = { 0.9f,0.0f,0.60f };
	Vector3 ChaseColor_ = { 2.5f,0.0f,0.00f };
};