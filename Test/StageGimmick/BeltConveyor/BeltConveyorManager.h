#pragma once

#include <list>
#include <memory>

#include "Player/Player.h"
#include "BeltConveyor.h"

class BeltConveyorManager {
public:
	void Initialize(uint32_t stageIndex);
	void Update();
	void Reset(uint32_t stageIndex);

	void Create(const BeltConveyor::Desc& desc);

	void SetPlayer(Player* player) { player_ = player; }

	const std::list<std::unique_ptr<BeltConveyor>>& GetBlocks() const { return beltConveyor_; }
	void Delete(BeltConveyor* beltConveyor);

	void LoadJson(uint32_t stageIndex);

	void Clear();

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Camera* camera_;
	Player* player_;
	std::list<std::unique_ptr<BeltConveyor>> beltConveyor_;
};