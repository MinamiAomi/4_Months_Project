#include "GameClearMovie.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"

void GameClearMovie::Update() {
	if (!isInitialize_) {
		isInitialize_ = true;

	}
	Movie::Update();
	player_;
	boss_;
	camera_;
	frame_++;
	if (frame_ >= kFullFrame) {
		isPlaying = false;
	}
}

void GameClearMovie::Reset()
{
	Movie::Reset();
}

void GameClearMovie::UpdateInitialize()
{
}
