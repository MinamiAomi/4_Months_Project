#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"
#include "Movie.h"


class GameStartMovie : public Movie {
public:

	const int kCloseFrame = 30;
	const int kRoarFrame = 180;

	const int kFullFrame = kCloseFrame + kRoarFrame;

	void Update() override;
	void Reset() override;
	bool GetIsEnd() { return isEnd_; }
	void ResetIsEnd() { isEnd_ = false; }
private:
	void UpdateInitialize();
	Vector3 saveCameraPos_;
	Vector3 closePos_;
	bool isEnd_ = false;
};