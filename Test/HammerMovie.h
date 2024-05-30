#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"
#include "Movie.h"


class HammerMovie : public Movie{
public:

	const int kZoomFrame = 40;
	const int kSwingFrame = 120;
	const int kBackFrame = 40;
	const int kFullFrame = kZoomFrame + kSwingFrame + kBackFrame;

	void Update() override;
	void Reset() override;
private:
	Vector3 curvePoint_[3];
};