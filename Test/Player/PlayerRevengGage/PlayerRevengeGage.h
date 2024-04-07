#pragma once

#include <cstdint>

#include "CharacterState.h"

class PlayerRevengeGage {
public:
	static const float kMaxRevengeBar;
	static const float kMaxRevengeCircle;

	void Initialize();
	void Update();
	void Reset();

	const float GetCurrentRevengeBarGage()const { return currentRevengeBarGage_; }
	const float GetCurrentRevengeCircleGage()const { return currentRevengeCircleGage_; }
private:
	float addGageBar_;
	float subGageBar_;
	float addGageCircle_;
	float subGageCircle_;
	float currentRevengeBarGage_;
	float currentRevengeCircleGage_;

};