#pragma once

#include <list>
#include "Trap/Trap.h"

#include "Engine/Math/Camera.h"
class Player;
class TrapManager {
public:
	void Initialize();
	void Update();

	void Create(const Vector3& position);
	
	void Reset();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	float velocity_;
	float offset_;

	const Camera* camera_;
	const Player* player_;
	std::list<std::unique_ptr<Trap>> traps_;
};