#include "GameClearMovie.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"


float GameClearMovie::t_ = 0.0f;

void GameClearMovie::Update() {
	if (!isInitialize_) {
		isInitialize_ = true;
		const Vector3 oneCameOffset = {-30.0f,33.0f,-60.0f};
		const Vector3 twoCameOffset = { 47.0f,37.0f,30.0f };
		const Vector3 threeCameOffset = { -1.0f,20.0f,57.0f };
		Vector3 bossVec = boss_->transform.worldMatrix.GetTranslate();
		saveZoomPos_[0] = bossVec + oneCameOffset;
		saveZoomPos_[1] = bossVec + twoCameOffset;
		saveZoomPos_[2] = bossVec + threeCameOffset;
		camera_->SetPosition(saveZoomPos_[0]);
	}

	Movie::Update();

	Vector3 cameraToBoss ;

	//1came
	float t = float(frame_) / kOneCameFrame;
	t = std::clamp(t, 0.0f, 1.0f);
	if (t < 1.0f) {
		t_ = 1.0f - t * 2.0f;
		t_ = std::clamp(t_, 0.0f, 1.0f);
		camera_->SetPosition(saveZoomPos_[0]);
		cameraToBoss = (boss_->transform.worldMatrix.GetTranslate() - camera_->GetPosition()).Normalize();
		ZoomInOut(saveZoomPos_[0], cameraToBoss, t);
	}
	else {
		//2came
		t = float(frame_ - kOneCameFrame) / kTwoCameFrame;
		t = std::clamp(t, 0.0f, 1.0f);
		if (t < 1.0f) {
			t_ = 1.0f - t * 2.0f;
			t_ = std::clamp(t_, 0.0f, 1.0f);
			camera_->SetPosition(saveZoomPos_[1]);
			cameraToBoss = (boss_->transform.worldMatrix.GetTranslate() - camera_->GetPosition()).Normalize();
			ZoomInOut(saveZoomPos_[1], cameraToBoss, t);
		}
		else {
			//3came
			t = float(frame_ - kOneCameFrame - kTwoCameFrame) / kThreeCameFrame;
			t = std::clamp(t, 0.0f, 1.0f);
			if (t < 1.0f) {
				t_ = 1.0f - t * 2.0f;
				t_ = std::clamp(t_, 0.0f, 1.0f);
				camera_->SetPosition(saveZoomPos_[2]);
				cameraToBoss = (boss_->transform.worldMatrix.GetTranslate() - camera_->GetPosition()).Normalize();
				ZoomInOut(saveZoomPos_[2], cameraToBoss, t);
			}
			else {
				//Camera
				t = float(frame_ - kOneCameFrame - kTwoCameFrame - kThreeCameFrame) / kRotateFrame;
				t = std::clamp(t, 0.0f, 1.0f);
				camera_->SetPosition(Vector3::Lerp(t, saveZoomPos_[2], { saveZoomPos_[2].x,saveZoomPos_[2].y,saveZoomPos_[2].z + 20.0f }));
				cameraToBoss = (boss_->transform.worldMatrix.GetTranslate() - camera_->GetPosition()).Normalize();
			}
		}
	}
	camera_->SetRotate(Quaternion::MakeLookRotation(cameraToBoss));
	camera_->UpdateMatrices();
	frame_++;
	if (frame_ >= kFullFrame) {
		isPlaying = false;
	}
}

void GameClearMovie::Reset()
{
	Movie::Reset();
	t_ = 0.0f;
}

void GameClearMovie::UpdateInitialize()
{
}
