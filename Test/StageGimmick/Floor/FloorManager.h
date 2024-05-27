#pragma once

#include <list>
#include <memory>

#include "Player/Player.h"
#include "Floor.h"

class FloorManager {
public:
	void Initialize();
	void Update();
	void Reset();

	void Create(const Floor::Desc& desc);

	void SetPlayer(const Player* player) { player_ = player; }

	const std::list<std::unique_ptr<Floor>>& GetFloors() const { return floors_; }
	void DeleteFloor(Floor* block);
	void Clear();

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Player* player_;
	const Camera* camera_;
	std::list<std::unique_ptr<Floor>> floors_;
};