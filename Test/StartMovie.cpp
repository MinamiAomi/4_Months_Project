#include "StartMovie.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"

void StartMovie::Update() {
	Movie::Update();
	player_;
	boss_;
	camera_;
	test--;
	test = std::clamp(test, 0, INT_MAX);
	if (test <= 0) {
		isPlaying = false;
	}
}

void StartMovie::Reset()
{
	Movie::Reset();
}
