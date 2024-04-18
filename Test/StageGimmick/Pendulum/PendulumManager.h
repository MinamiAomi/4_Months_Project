#pragma once

#include <list>
#include <memory>

#include "Player/Player.h"
#include "Pendulum.h"

class PendulumManager {
public:
	void Initialize(uint32_t stageIndex);
	void Update();
	void Reset(uint32_t stageIndex);

	void Create(Pendulum::Desc desc);

	void SetPlayer(const Player* player) { player_ = player; }

	const std::list<std::unique_ptr<Pendulum>>& GetPendulums() const { return pendulums_; }
	void DeletePendulum(Pendulum* block);

	void LoadJson(uint32_t stageIndex);

	void Clear();

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Camera* camera_;
	const Player* player_;
	std::list<std::unique_ptr<Pendulum>> pendulums_;
};