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
	void SetIsMove(bool flag) { isMove_ = flag; }

	void AddGage();

	const float GetCurrentRevengeBarGage()const { return currentRevengeBarGage_; }
	const float GetCurrentRevengeCircleGage()const { return currentRevengeCircleGage_; }
private:
	float addGage_;
	float subGageBar_;
	float subGageCircle_;
	float currentRevengeBarGage_;
	float currentRevengeCircleGage_;

	// Debug
	bool isMove_;
	bool isUpdate_;
};