#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"
#include "Movie.h"


class GameClearMovie : public Movie {
public:

	static float t_;

	const int kOneCameFrame = 20;
	const int kTwoCameFrame = 20;
	const int kThreeCameFrame = 20;
	const int kRotateFrame = 100;
	const int kFullFrame = kOneCameFrame + kTwoCameFrame + kThreeCameFrame + kRotateFrame;
	void Update() override;
	void Reset() override;
	void UpdateInitialize();
private:
	
};