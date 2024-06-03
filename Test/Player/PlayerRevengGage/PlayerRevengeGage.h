#pragma once

#include <cstdint>

#include "CharacterState.h"

class Boss;
class Player;

class PlayerRevengeGage {
public:
	static const float kMaxRevengeBar;

	void Initialize(const Boss* boss,const Player* player);
	void Update();
	void Reset();
	void SetIsMove(bool flag) { isMove_ = flag; }

	void AddGage();

	const float GetCurrentRevengeBarGage()const { return currentRevengeBarGage_; }
	void SetCurrentRevengeBarGage(float gage) { currentRevengeBarGage_ = gage; }
private:

	const Boss* boss_ = nullptr;
	const Player* player_ = nullptr;

	float addCoin_;
	float addGageBar_;
	float subGageBar_;
	float currentRevengeBarGage_;

	// Debug
	bool isMove_;
	bool isUpdate_;
};