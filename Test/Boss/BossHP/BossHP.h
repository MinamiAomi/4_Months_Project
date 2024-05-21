#pragma once

#include <cstdint>

class BossHP {
public:
	static const uint32_t kMaxHP = 20;

	void Initialize();
	void Update();

	void Reset();

	void AddPlayerHitHP() { currentHP_ -= playerHitDamage_; }
	void AddBallHitHP() { currentHP_ -= ballHitDamage_; }

	const uint32_t GetCurrentHP() const { return currentHP_; }
private:
	uint32_t currentHP_;

	uint32_t ballHitDamage_;
	uint32_t playerHitDamage_;
};