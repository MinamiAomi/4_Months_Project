#include "HammerMovie.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"
#include "Graphics/PostEffect.h"
 
void HammerMovie::Initialize(Player* player, Boss* boss, Camera* camera, const StageCamera* stageCamera)
{
	player_ = player;
	boss_ = boss;
	camera_ = camera;
	stageCamera_ = stageCamera;

	isPlaying = false;
	isSetCamera_ = false;
	frame_ = 0;

	se_ = std::make_unique<AudioSource>();
	(*se_) = ResourceManager::GetInstance()->FindSound("hammerAttack");
}

void HammerMovie::Update() {
	
	Movie::Update();
	if (!isInitialize_) {
		isInitialize_ = true;
		camera_->SetPosition(stageCamera_->GetCamera()->GetPosition());
		curvePoint_[0] = player_->transform.worldMatrix.GetTranslate();
		boss_->transform.worldMatrix.GetTranslate();
		
		curvePoint_[2] = boss_->transform.translate;
		curvePoint_[2].y += 10.0f;
		curvePoint_[2].z -= 24.0f;

		curvePoint_[1] = curvePoint_[0] + ((curvePoint_[2] - curvePoint_[0]) / 2.0f);

		curvePoint_[1].y += 30.0f;
		Vector3 playerToBoss = (boss_->transform.worldMatrix.GetTranslate() - player_->transform.worldMatrix.GetTranslate());
		initializeQuaternion_ = player_->transform.rotate;
		player_->transform.rotate = Quaternion::MakeLookRotation(playerToBoss);
		saveQuaternion_ = player_->transform.rotate;
		isPlaySe_ = false;
		if (player_->transform.worldMatrix.GetTranslate().x < 0.0f) {
			isLeft_ = true;
		}
		else {
			isLeft_ = false;
		}
	}

	Vector3 cameraToPlayer = (player_->transform.worldMatrix.GetTranslate() - camera_->GetPosition()).Normalize();
	Vector3 pos = stageCamera_->GetCamera()->GetPosition();
	//近づく
	float t = float(frame_) / kZoomFrame;
	t = std::clamp(t, 0.0f, 1.0f);

	Vector3 playerOffset = { 8.0f,17.0f,-14.0f };
	if (isLeft_) {
		playerOffset.x *= -1.0f;
	}

 	if (t < 1.0f) {
		camera_->SetPosition(Vector3::Lerp(t, pos, player_->transform.worldMatrix.GetTranslate() + playerOffset));
		player_->transform.translate = Vector3::QuadraticBezierCurve(t, curvePoint_[0], curvePoint_[1], curvePoint_[2]);
		player_->transform.rotate = Quaternion::MakeForXAxis(std::lerp(0.0f,420.0f * Math::ToRadian,t)) * saveQuaternion_;
		PostEffect::blurT_ = 1.0f - t;
	}
	else {
		//stop
		PostEffect::blurT_ = 0.0f;
		t = float(frame_ - kZoomFrame) / kStopFrame;
		t = std::clamp(t, 0.0f, 1.0f);
		if (t < 1.0f) {
			if (!isPlaySe_) {
				isPlaySe_ = true;
				se_->Play();
				se_->SetVolume(1.0f);
			}
			camera_->Shake({ 0.5f,0.5f,0.5f });
			
		}
		else {
			//戻る
			PostEffect::blurT_ = 1.0f;
			t = float(frame_ - kZoomFrame - kStopFrame) / kBackFrame;
			t = std::clamp(t, 0.0f, 1.0f);
			camera_->SetPosition(Vector3::Lerp(t, player_->transform.worldMatrix.GetTranslate() + playerOffset, stageCamera_->GetCamera()->GetPosition()));
			player_->transform.translate = Vector3::QuadraticBezierCurve(t, curvePoint_[2], curvePoint_[1], curvePoint_[0]);
			player_->transform.rotate = Quaternion::MakeForXAxis(std::lerp(420.0f * Math::ToRadian, 0.0f , t)) * saveQuaternion_;
		}
	}

	camera_->SetRotate(Quaternion::MakeLookRotation(cameraToPlayer));
	camera_->UpdateMatrices();
	player_->UpdateTransform();
	Vector3 result = camera_->GetPosition();
	frame_++;
	frame_ = std::clamp(frame_, 0, INT_MAX);
	if (frame_ >= kFullFrame) {
		PostEffect::blurT_ = 0.0f;
		isPlaying = false;
		player_->transform.rotate = initializeQuaternion_;
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
