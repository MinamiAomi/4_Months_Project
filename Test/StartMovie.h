#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"
#include "Movie.h"


class StartMovie : public Movie{
public:

	const int kFullFrame = 120;

	void Update() override;
	void Reset() override;
private:
	void UpdateInitialize();
};