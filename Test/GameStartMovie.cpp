#include "GameStartMovie.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"
#include "CharacterState.h"

void GameStartMovie::Update() {
	player_->GetModelInstance().SetColor({1.0f,1.0f,1.0f});
	Movie::Update();
	if (!isInitialize_) {
		isInitialize_ = true;
		player_->GetTransform().rotate = Quaternion::MakeForYAxis(180.0f * Math::ToRadian);
		closePos_ = { 0.0f,19.0f,-77.0f };
		saveCameraPos_ = stageCamera_->GetCamera()->GetPosition();
	}	

	//近づく
	float t = float(frame_) / kCloseFrame;
	t = std::clamp(t, 0.0f, 1.0f);
	if (t < 1.0f) {
		camera_->SetPosition(Vector3::Lerp(t, saveCameraPos_, boss_->transform.worldMatrix.GetTranslate() + closePos_));
	}
	else {
		//咆哮 カメラ戻る
		t = float(frame_ - kCloseFrame) / kRoarFrame;
		t = std::clamp(t, 0.0f, 1.0f);
		if (t < 1.0f) {
			camera_->Shake({ 0.5f,0.5f,0.5f });
			camera_->SetPosition(Vector3::Lerp(t, boss_->transform.worldMatrix.GetTranslate() + closePos_, saveCameraPos_));
			boss_->transform.rotate = Quaternion::MakeForYAxis(std::lerp(180.0f * Math::ToRadian, 0.0f, t));
		}
		else {
			
		}
	}

	
	boss_->UpdateTransform();
	camera_->UpdateMatrices();
	frame_++;

	frame_ = std::clamp(frame_, 0, INT_MAX);
	if (frame_ >= kFullFrame) {
		isPlaying = false;
		isEnd_ = true;
		Character::SetNextScene(Character::kRunAway);
	}
}

void GameStartMovie::Reset()
{
	Movie::Reset();
	boss_->transform.rotate = Quaternion::MakeForYAxis(0.0f * Math::ToRadian);
}

void GameStartMovie::UpdateInitialize()
{
}
