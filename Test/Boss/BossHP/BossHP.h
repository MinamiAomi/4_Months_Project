#pragma once

#include <cstdint>

class BossHP {
public:
	static const uint32_t kMaxHP = 20;

	void Initialize();
	void Update();

	void Reset();

	void AddPlayerHitHP() {
		currentHP_ -= playerHitDamage_; 
		currentHP_ = std::clamp(currentHP_, 0, INT_MAX);
	}
	void AddBallHitHP() { 
		currentHP_ -= ballHitDamage_;
		currentHP_ = std::clamp(currentHP_, 0, INT_MAX);
	}

	const uint32_t GetCurrentHP() const { return currentHP_; }
private:
	int32_t currentHP_;

	uint32_t ballHitDamage_;
	uint32_t playerHitDamage_;
};