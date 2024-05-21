#include "StartMovie.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"

void StartMovie::Update() {
	Vector3 playerPos = player_->GetWorldMatrix().GetTranslate();
	if (!isInitialize_) {
		isInitialize_ = true;
		whileVec_ = (Vector3{ playerPos.x,playerPos.y,10.0f + playerPos.z } - stageCamera_->GetCamera()->GetPosition()) / 2.0f;
		lerpCenterPos_ = stageCamera_->GetCamera()->GetPosition() + whileVec_;
	}
	float t =  frame_ / float(kFullFrame);
	t = std::clamp(t, 0.0f, 1.0f);
	Vector3 axis = Vector3::Cross(whileVec_, {0.0f,1.0f,0.0f});
	Vector3 newUpVector = Vector3::Cross(axis, whileVec_);
	Vector3 rotateVec = Quaternion::MakeFromAngleAxis(std::lerp(0.0f, -180.0f * Math::ToRadian, t), newUpVector)* whileVec_;
	camera_->SetPosition(lerpCenterPos_ + rotateVec);
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
