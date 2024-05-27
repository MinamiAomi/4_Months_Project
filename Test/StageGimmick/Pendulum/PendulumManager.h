#pragma once

#include <list>
#include <memory>

#include "Player/Player.h"
#include "Pendulum.h"

class PendulumManager {
public:
	void Initialize();
	void Update();
	void Reset();

	void Create(Pendulum::Desc desc, uint32_t index);

	void SetPlayer(const Player* player) { player_ = player; }

	std::list<std::unique_ptr<Pendulum>>& GetPendulums() { return pendulums_; }
	void DeletePendulum(Pendulum* block);

	void Clear();

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Camera* camera_;
	const Player* player_;
	std::list<std::unique_ptr<Pendulum>> pendulums_;
};