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

	void Create(
		const Vector3& scale, const Vector3& rotate,
		const Vector3& ballScale, const Vector3& ballRotate,
		const Vector3& pos, float length, float speed, float angle);

	void SetPlayer(const Player* player) { player_ = player; }

	const std::list<std::unique_ptr<Pendulum>>& GetPendulums() const { return pendulums_; }
	void DeletePendulum(Pendulum* block);

	void LoadJson(uint32_t stageIndex);

	void Clear();
private:
	const Player* player_;
	std::list<std::unique_ptr<Pendulum>> pendulums_;
};