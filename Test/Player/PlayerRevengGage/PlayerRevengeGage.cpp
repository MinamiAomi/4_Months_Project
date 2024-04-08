#include "PlayerRevengeGage.h"

#include "Input/Input.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"

const float PlayerRevengeGage::kMaxRevengeBar = 100.0f;
const float PlayerRevengeGage::kMaxRevengeCircle = 50.0f;

void PlayerRevengeGage::Initialize() {
	Reset();

	JSON_OPEN("Resources/Data/Player/PlayerRevengeGage.json");
	JSON_OBJECT("PlayerRevengeGage");
	JSON_LOAD(addGageBar_);
	JSON_LOAD(subGageBar_);
	JSON_LOAD(addGageCircle_);
	JSON_LOAD(subGageCircle_);
	JSON_CLOSE();
}

void PlayerRevengeGage::Update() {
	switch (characterState_) {
	case Character::State::kChase:
	{
		if (isMove_) {
			currentRevengeBarGage_ -= subGageBar_;
		}
		if (currentRevengeBarGage_ <= 0) {
			characterState_ = Character::State::kRunAway;
			currentRevengeBarGage_ = 0.0f;
			currentRevengeCircleGage_ = 0.0f;
		}
	}
	break;
	case Character::State::kRunAway:
	{
		if (isMove_) {
			if (currentRevengeBarGage_ < kMaxRevengeBar) {
				currentRevengeBarGage_ += addGageBar_;
			}
			else if (currentRevengeCircleGage_ < kMaxRevengeCircle) {
				currentRevengeCircleGage_ += addGageBar_;
			}
		}

		if ((currentRevengeBarGage_ >= kMaxRevengeBar) &&
			(Input::GetInstance()->IsKeyTrigger(DIK_LSHIFT) || (Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_B))) {
			characterState_ = Character::State::kChase;

		}
	}
	break;
	default:
		break;
	}
	currentRevengeBarGage_ = std::clamp(currentRevengeBarGage_, 0.0f, kMaxRevengeBar);
	currentRevengeCircleGage_ = std::clamp(currentRevengeCircleGage_, 0.0f, kMaxRevengeCircle);


#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Player")) {
		if (ImGui::TreeNode("PlayerRevengeGage")) {
			ImGui::DragFloat("currentRevengeBarGage_", &currentRevengeBarGage_, 0.1f);
			ImGui::DragFloat("currentRevengeCircleGage_", &currentRevengeCircleGage_, 0.1f);
			ImGui::DragFloat("addGageBar_", &addGageBar_, 0.1f);
			ImGui::DragFloat("subGageBar_", &subGageBar_, 0.1f);
			ImGui::DragFloat("addGageCircle_", &addGageCircle_, 0.1f);
			ImGui::DragFloat("subGageCircle_", &subGageCircle_, 0.1f);

			if (ImGui::Button("Save")) {
				JSON_OPEN("Resources/Data/Player/PlayerRevengeGage.json");
				JSON_OBJECT("PlayerRevengeGage");
				JSON_SAVE(addGageBar_);
				JSON_SAVE(subGageBar_);
				JSON_SAVE(addGageCircle_);
				JSON_SAVE(subGageCircle_);
				JSON_CLOSE();
			}
			ImGui::TreePop();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
}

void PlayerRevengeGage::Reset() {
	isMove_ = true;
	currentRevengeBarGage_ = 0.0f;
	currentRevengeCircleGage_ = 0.0f;
	characterState_ = Character::State::kRunAway;
}
