#pragma once

#include <cstdint>

class BossHP {
public:
	static const int32_t kMaxHP = 20;

	void Initialize();
	void Update();

	void Reset();

	void AddPlayerHitHP();
	void AddBallHitHP();

	const int32_t GetCurrentHP() const { return currentHP_; }
private:
	int32_t currentHP_;

	int32_t ballHitDamage_;
	int32_t playerHitDamage_;
};