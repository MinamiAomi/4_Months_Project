#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"
#include "Movie.h"


class GameOverMovie : public Movie {
public:

	const int kCloseFrame = 30;
	const int kShakeFrame = 5;
	const int kStopFrame = 10;
	const int kDropFrame = 90;

	const int kFullFrame = kCloseFrame + kShakeFrame + kStopFrame + kDropFrame;

	void Update() override;
	void Reset() override;
private:
	void UpdateInitialize();
	Vector3 savePlayerPos_;
	int shakeFrame_ = kShakeFrame;
};