#pragma once

#include <list>
#include <memory>

#include "Player/Player.h"
#include "Floor.h"

class FloorManager {
public:
	void Initialize(uint32_t stageIndex);
	void Update();
	void Reset(uint32_t stageIndex);

	void Create(const StageGimmick::Desc& desc);

	void SetPlayer(const Player* player) { player_ = player; }

	const std::list<std::unique_ptr<Floor>>& GetFloors() const { return floors_; }
	void DeleteFloor(Floor* block);

	void LoadJson(uint32_t stageIndex);

	void Clear();
private:
	const Player* player_;
	std::list<std::unique_ptr<Floor>> floors_;
};