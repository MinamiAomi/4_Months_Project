#pragma once

#include <list>
#include "Trap/Trap.h"

#include "Engine/Math/Camera.h"
class Player;
class Boss;
class TrapManager {
public:
	void Initialize();
	void Update();

	void Create(const Vector3& position);
	
	void Clear();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetCamera(const Camera* camera) { camera_ = camera; }

private:
	Trap::Desc desc_;

	const Camera* camera_;
	const Player* player_;
	const Boss* boss_;
	std::list<std::unique_ptr<Trap>> traps_;
};