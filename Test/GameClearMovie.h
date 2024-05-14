#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"
#include "Movie.h"


class GameClearMovie : public Movie {
public:
	void Update() override;
	void Reset() override;
private:

};