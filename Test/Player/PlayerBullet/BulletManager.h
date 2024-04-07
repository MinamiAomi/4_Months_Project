#pragma once

#include <array>
#include <memory>

#include "Math/MathUtils.h"
#include "PlayerBullet.h"

class BulletManager {
public:
	static const uint32_t kMaxBullet = 20;

	void Initialize();
	void Update(const Vector3& pos);
	void Create(const Vector3& pos);
	void Reset();
private:
	std::array<std::unique_ptr<PlayerBullet>, kMaxBullet> bullets_;
	Vector3 velocity_;
	uint32_t time_;
	uint32_t coolTime_;
};