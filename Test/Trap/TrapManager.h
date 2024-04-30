#pragma once

#include <list>
#include "Trap/Trap.h"

#include "Engine/Math/Camera.h"
class TrapManager {
public:
	void Initialize();
	void Update();

	void Create(const Vector3& position);
	
	void Reset();

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	float velocity_;

	const Camera* camera_;
	std::list<std::unique_ptr<Trap>> traps_;
};