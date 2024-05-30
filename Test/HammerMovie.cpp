#include "HammerMovie.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"
#include "Graphics/PostEffect.h"
 
void HammerMovie::Update() {
	
	Movie::Update();
	if (!isInitialize_) {
		isInitialize_ = true;
		camera_->SetPosition(stageCamera_->GetCamera()->GetPosition());
		curvePoint_[0] = player_->transform.worldMatrix.GetTranslate();
		boss_->transform.worldMatrix.GetTranslate();
		/*curvePoint_[1] = curvePoint_[0] + (player_->transform.worldMatrix.GetTranslate() - );
		curvePoint_[0]*/
	}

	Vector3 cameraToPlayer = (player_->transform.worldMatrix.GetTranslate() - camera_->GetPosition()).Normalize();
	Vector3 pos = stageCamera_->GetCamera()->GetPosition();
	//近づく
	float t = float(frame_) / kZoomFrame;
	t = std::clamp(t, 0.0f, 1.0f);

	Vector3 playerOffset = { 4.0f,9.0f,0.0f };
	if (player_->transform.worldMatrix.GetTranslate().x < 0.0f) {
		playerOffset.x *= -1.0f;
	}

	if (t < 1.0f) {
		camera_->SetPosition(Vector3::Lerp(t, pos, player_->transform.worldMatrix.GetTranslate() + playerOffset));
		//Vector3::QuadraticBezierCurve(t, curveStart_,);
		PostEffect::blurT_ = 1.0f - t;
	}
	else {
		//振る
		PostEffect::blurT_ = 0.0f;
		t = float(frame_ - kZoomFrame) / kSwingFrame;
		t = std::clamp(t, 0.0f, 1.0f);
		if (t < 1.0f) {
			
			
			
		}
		else {
			//戻る
			PostEffect::blurT_ = 0.0f;
			t = float(frame_ - kZoomFrame - kSwingFrame) / kBackFrame;
			t = std::clamp(t, 0.0f, 1.0f);
			camera_->SetPosition(Vector3::Lerp(t, player_->transform.worldMatrix.GetTranslate() + playerOffset, stageCamera_->GetCamera()->GetPosition()));
		}
	}

	camera_->SetRotate(Quaternion::MakeLookRotation(cameraToPlayer));
	camera_->UpdateMatrices();
	player_->UpdateTransform();
	Vector3 result = camera_->GetPosition();
	frame_++;
	frame_ = std::clamp(frame_, 0, INT_MAX);
	if (frame_ >= kFullFrame) {
		isPlaying = false;
		if (boss_->GetIsFirstHit()) {
			Character::SetNextScene(Character::State::kRunAway);
		}
		boss_->SetIsFirstHit(true);
	}
}

void HammerMovie::Reset()
{
	Movie::Reset();
}
