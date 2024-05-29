#include "HammerMovie.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"

void HammerMovie::Update() {
	Vector3 playerPos = player_->GetWorldMatrix().GetTranslate();
	if (!isInitialize_) {
		isInitialize_ = true;
	}
	float t =  frame_ / float(kFullFrame);
	t = std::clamp(t, 0.0f, 1.0f);
	
	

	
	frame_ = std::clamp(frame_, 0, INT_MAX);
	if (frame_ >= kFullFrame) {
		isPlaying = false;
	}
}

void HammerMovie::Reset()
{
	Movie::Reset();
}
