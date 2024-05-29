#pragma once

#include <cstdint>
#include <algorithm>
#include <climits>


class Camera;
class BossHP {
public:
	static const int32_t kMaxHP = 20;

	void Initialize();
	void Update();

	void Reset();

	void AddPlayerHitHP();
	void AddBallHitHP();

	const int32_t GetCurrentHP() const { return currentHP_; }

	void SetCamera(Camera* camera) { camera_ = camera; }
	//
	uint32_t totalBallShakeFrame_;
	uint32_t shakeFrame_;
	bool isShake_;
	void Shake();

	//
private:
	int32_t currentHP_;

	int32_t ballHitDamage_;
	int32_t playerHitDamage_;

	//
	Camera* camera_ = nullptr;
	//
};