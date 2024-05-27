#pragma once

#include <list>
#include <memory>

#include "Player/Player.h"
#include "BeltConveyor.h"

class BeltConveyorManager {
public:
	void Initialize();
	void Update();
	void Reset();

	void Create(const BeltConveyor::Desc& desc, uint32_t index);

	void SetPlayer(Player* player) { player_ = player; }

	std::list<std::unique_ptr<BeltConveyor>>& GetBlocks() { return beltConveyor_; }
	void Delete(BeltConveyor* beltConveyor);

	void Clear();

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Camera* camera_;
	Player* player_;
	std::list<std::unique_ptr<BeltConveyor>> beltConveyor_;
};