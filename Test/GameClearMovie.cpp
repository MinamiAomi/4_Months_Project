#include "GameClearMovie.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"

void GameClearMovie::Update() {
	Movie::Update();
	player_;
	boss_;
	camera_;
	test--;
	if (test <= 0) {
		isPlaying = false;
	}
}

void GameClearMovie::Reset()
{
	Movie::Reset();
}
