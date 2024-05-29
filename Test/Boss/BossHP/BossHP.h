#pragma once

#include <cstdint>
#include <algorithm>
#include <climits>

#include <CameraManager/StageCamera/StageCamera.h>

class BossHP {
public:
	static const int32_t kMaxHP = 20;

	void Initialize(Camera* camera);
	void Update();

	void Reset();

	void AddPlayerHitHP();
	void AddBallHitHP();

	const int32_t GetCurrentHP() const { return currentHP_; }
	//
	const int kShakeframe = 5;
	 int shakeframe;
	 bool isShake;
	 void Shake();



	//
private:
	int32_t currentHP_;

	int32_t ballHitDamage_;
	int32_t playerHitDamage_;

	//
	Camera* camera_ = nullptr;
	const StageCamera* stageCamera_ = nullptr;
	//
};