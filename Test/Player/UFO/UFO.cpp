#include "UFO.h"
#include "Framework/ResourceManager.h"
#include "CharacterState.h"
#include "Player/Player.h"
#include "Boss/Boss.h"
#include "GameSpeed.h"

void UFO::Initialize(Player* player,const Boss* boss) {
	player_ = player;
	boss_ = boss;
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("ufo"));
	model_->SetIsActive(false);
	model_->SetWireFrameMask(true);

	transform.SetParent(&player_->transform);
	transform.translate.y = player_->GetModelInstance().GetModel()->GetMeshes()[0].maxVertex.y + 9.0f;
	transform.isParentRotate = false;
	isSaveDisAppearPos_ = false;
	isFreePlayer_ = true;
}

void UFO::Update() {

	if (model_->IsActive()) {

		float t = float(frame) / backFrame;
		t = std::clamp(t, 0.0f, 1.0f);
		//戻る
		if (t < 1.0f) {
			if (Character::currentCharacterState_ == Character::kChase) {
				savePoint_[2].z += GameSpeed::GetGameSpeed();
			}
			if (Character::currentCharacterState_ == Character::kRunAway) {
				savePoint_[2].z -= GameSpeed::GetGameSpeed();
			}
			player_->transform.translate = Vector3::QuadraticBezierCurve(t, savePoint_[0], savePoint_[1], savePoint_[2]);
			player_->transform.rotate = Quaternion::MakeFromEulerAngle({-1.0f * frame * 2.0f * Math::ToRadian,1.0f * frame * 2.0f * Math::ToRadian,-1.0f * frame * 2.0f * Math::ToRadian });
		}
		else {
			//待機 プレイヤーがジャンプしたときこの待機はスキップ
			t = float(frame - backFrame) / waitFrame;
			t = std::clamp(t, 0.0f, 1.0f);
			if (t < 1.0f) {
				player_->Jump();
				if (Character::currentCharacterState_ == Character::kChase) {
					player_->transform.translate.z += GameSpeed::GetGameSpeed();
				}
				if (Character::currentCharacterState_ == Character::kRunAway) {
					player_->transform.translate.z -= GameSpeed::GetGameSpeed();
				}
				if (!player_->GetCanFirstJump()) {
					isFreePlayer_ = true;
					frame = backFrame + waitFrame;
				}
				player_->transform.rotate = Quaternion::MakeFromEulerAngle({ -1.0f * frame * 2.0f * Math::ToRadian,1.0f * frame * 2.0f * Math::ToRadian,-1.0f * frame * 2.0f * Math::ToRadian });
			}
			else {
				if (!isSaveDisAppearPos_) {
					isSaveDisAppearPos_ = true;
					transform.SetParent(nullptr);
					saveDisAppearPos_ = transform.translate;
					player_->transform.rotate = Quaternion::identity;
					isFreePlayer_ = true;
				}
				//消える
				t = float(frame - backFrame - waitFrame) / disAppearFrame_;
				t = std::clamp(t, 0.0f, 1.0f);
				if (t < 1.0f) {
					transform.translate = Vector3::Lerp(t, saveDisAppearPos_, { saveDisAppearPos_.x,27.0f,saveDisAppearPos_.z });
				}
			}
		}

		frame++;
		frame = std::clamp(frame, 0, INT_MAX);
		transform.rotate = Quaternion::MakeFromEulerAngle({ 0.0f,frame * 2.0f * Math::ToRadian,0.0f });
		transform.UpdateMatrix();
		model_->SetWorldMatrix(transform.worldMatrix);

		if (frame >= kFullFrame) {
			model_->SetIsActive(false);
		}
	}
}

void UFO::UpdateTransform()
{
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}

void UFO::SetIsActive()
{
	model_->SetIsActive(true);
	savePoint_[0] = player_->transform.worldMatrix.GetTranslate();

	savePoint_[1] = savePoint_[0];
	savePoint_[1].y = 10.0f;

	savePoint_[2] = boss_->transform.translate;
	savePoint_[2].y = 15.0f;

	transform.translate = Vector3::zero;
	transform.SetParent(&player_->transform);
	transform.translate.x = 0.0f;
	transform.translate.z = 0.0f;

	if (Character::currentCharacterState_ == Character::kChase) {
		savePoint_[2].z = savePoint_[2].z - player_->GetChaseLimitLine();
		transform.translate.y = player_->GetModelInstance().GetModel()->GetMeshes()[0].maxVertex.y + 9.0f;
	}
	else if (Character::currentCharacterState_ == Character::kRunAway) {
		savePoint_[2].z = savePoint_[2].z - player_->GetRunAwayLimitLine() / 2.0f;
		transform.translate.y = player_->GetModelInstance().GetModel()->GetMeshes()[0].maxVertex.y + 3.0f;
	}

	isSaveDisAppearPos_ = false;

	
	frame = 0;
	isFreePlayer_ = false;
};


void UFO::SetIsInactive()
{
	model_->SetIsActive(false);
}
