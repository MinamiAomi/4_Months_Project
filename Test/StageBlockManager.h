#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Math/MathUtils.h"
#include "Graphics/Model.h"
#include "Player/Player.h"

#include "StageBlock.h"

class StageBlockManager :
	public GameObject {
public:
	void Initialize();
	void Update();
	void Appear();

	void SetPlayer(const Player* player) { player_ = player; }
private:
	void UpdateTransform();

	const Player* player_;

	std::vector<StageBlock> stageBlocks_;
	int blockAppear_ = 0;
};