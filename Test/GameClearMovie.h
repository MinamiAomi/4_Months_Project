#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"
#include "Movie.h"


class GameClearMovie : public Movie {
public:

	const int kFullFrame = 120;

	void Update() override;
	void Reset() override;
	void UpdateInitialize();
private:
	
};