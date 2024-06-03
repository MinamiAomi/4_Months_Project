#include "GameOverMovie.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"

void GameOverMovie::Update() {
	player_->GetModelInstance().SetColor({1.0f,1.0f,1.0f});

	if (!isInitialize_) {
		isClearGameOver = true;
		isInitialize_ = true;
		savePlayerPos_ = player_->GetWorldMatrix().GetTranslate();
		player_->GetTransform().rotate = Quaternion::MakeForYAxis(180.0f * Math::ToRadian);
	}

	camera_->SetPosition(stageCamera_->GetCamera()->GetPosition());

	Vector3 colPos = camera_->GetPosition();
	colPos.z += camera_->GetNearClip() + 1.0f;

	//close
	float t = float(frame_) / kCloseFrame;
	t = std::clamp(t, 0.0f, 1.0f);
	if (t < 1.0f) {
		player_->GetTransform().translate = Vector3::Lerp(t, savePlayerPos_, colPos);
		player_->GetTransform().rotate = player_->GetTransform().rotate * Quaternion::MakeForZAxis(40.0f * Math::ToRadian);

	}
	else {
		//くっつく
		t = float(frame_ - kCloseFrame) / kShakeFrame;
		t = std::clamp(t, 0.0f, 1.0f);
		if (t < 1.0f) {
			camera_->Shake({ 0.5f,0.5f,0.5f });
		}
		else {
			//静止
			t = float(frame_- kCloseFrame - kShakeFrame) / kStopFrame;
			t = std::clamp(t, 0.0f, 1.0f);
			if (t < 1.0f) {
				
			}
			else {
				//静止
				t = float(frame_ - kCloseFrame - kShakeFrame - kStopFrame) / kDropFrame;
				t = std::clamp(t, 0.0f, 1.0f);
				if (t < 1.0f) {
					player_->GetTransform().translate.y -= 0.04f;
				}
				else {

				}
			}
		}
	}

	player_->UpdateTransform();
	camera_->UpdateMatrices();
	frame_++;

	frame_ = std::clamp(frame_, 0, INT_MAX);
	if (frame_ >= kFullFrame) {
		isPlaying = false;
	}
}

void GameOverMovie::Reset()
{
	Movie::Reset();
}

void GameOverMovie::UpdateInitialize()
{
}
