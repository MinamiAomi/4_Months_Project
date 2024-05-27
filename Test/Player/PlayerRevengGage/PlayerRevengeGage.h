#pragma once

#include <cstdint>

#include "CharacterState.h"

class PlayerRevengeGage {
public:
	static const float kMaxRevengeBar;

	void Initialize();
	void Update();
	void Reset();
	void SetIsMove(bool flag) { isMove_ = flag; }

	void AddGage();

	const float GetCurrentRevengeBarGage()const { return currentRevengeBarGage_; }
	void SetCurrentRevengeBarGage(float gage) { currentRevengeBarGage_ = gage; }
private:
	float addCoin_;
	float addGageBar_;
	float subGageBar_;
	float currentRevengeBarGage_;

	// Debug
	bool isMove_;
	bool isUpdate_;
};