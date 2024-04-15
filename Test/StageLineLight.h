#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Math/MathUtils.h"
#include "Graphics/Model.h"
#include "Player/Player.h"

#include "Graphics/LightManager.h"

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
};