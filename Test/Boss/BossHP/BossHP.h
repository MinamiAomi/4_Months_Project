#pragma once

#include <cstdint>

class BossHP {
public:
	static const uint32_t kMaxHP = 20;

	void Initialize();
	void Update();

	void Reset();

	void AddHP(int32_t add) { currentHP_ += add; }

	const uint32_t GetCurrentHP() const { return currentHP_; }
private:
	uint32_t currentHP_;
};