#include "StartMovie.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"

void StartMovie::Update() {
	if (!isInitialize_) {
		isInitialize_ = true;
	}
	player_;
	boss_;
	camera_;
	stageCamera_;
	Vector3 playerPos = player_->GetWorldMatrix().GetTranslate();
	float t =  frame_ / float(kFullFrame);
	t = std::clamp(t, 0.0f, 1.0f);
	camera_->SetPosition(Vector3::Lerp(t, { playerPos.x,playerPos.y,10.0f + playerPos.z }, stageCamera_->GetCamera()->GetPosition()));
	Quaternion q = Quaternion::MakeForYAxis(180.0f * Math::ToRadian);
	camera_->SetRotate(Quaternion::Lerp(t, q, stageCamera_->GetCamera()->GetRotate()));
	camera_->UpdateMatrices();

	if (!SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {
		frame_++;
	}
	
	frame_ = std::clamp(frame_, 0, INT_MAX);
	if (frame_ >= kFullFrame) {
		isPlaying = false;
	}
}

void StartMovie::Reset()
{
	Movie::Reset();
}

void StartMovie::UpdateInitialize()
{
}
