#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"
#include "Movie.h"


class HammerMovie : public Movie{
public:

	const int kZoomFrame = 20;
	const int kSwingFrame = 60;
	const int kFullFrame = kZoomFrame;

	void Update() override;
	void Reset() override;
private:
};