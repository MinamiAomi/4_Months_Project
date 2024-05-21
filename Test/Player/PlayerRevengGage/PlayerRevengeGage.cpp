#include "PlayerRevengeGage.h"

#include "Input/Input.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"

const float PlayerRevengeGage::kMaxRevengeBar = 100.0f;

void PlayerRevengeGage::Initialize() {
	Reset();

	JSON_OPEN("Resources/Data/Player/PlayerRevengeGage.json");
	JSON_OBJECT("PlayerRevengeGage");
	JSON_LOAD(addCoin_);
	JSON_LOAD(addGageBar_);
	JSON_LOAD(subGageBar_);
	JSON_CLOSE();
}

void PlayerRevengeGage::Update() {
	switch (Character::currentCharacterState_) {
	case Character::State::kChase:
	{
		if (isMove_) {
			currentRevengeBarGage_ -= subGageBar_;
		}
		if (currentRevengeBarGage_ <= 0) {
			currentRevengeBarGage_ = 0.0f;
		}
	}
	break;
	case Character::State::kRunAway:
	{
		if (isMove_) {
			if (currentRevengeBarGage_ < kMaxRevengeBar) {
				currentRevengeBarGage_ += addGageBar_;
			}
		}
	}
	break;
	default:
		break;
	}
	currentRevengeBarGage_ = std::clamp(currentRevengeBarGage_, 0.0f, kMaxRevengeBar);


#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Player")) {
		ImGui::DragFloat("現在の復讐ゲージ", &currentRevengeBarGage_, 0.1f);
		if (ImGui::TreeNode("PlayerRevengeGage")) {
			ImGui::Checkbox("isUpdate", &isUpdate_);
			ImGui::DragFloat("リベンジコイン取得", &addCoin_, 0.1f);
			ImGui::DragFloat("増加値", &addGageBar_, 0.1f);
			ImGui::DragFloat("減少値", &subGageBar_, 0.1f);

			if (ImGui::Button("Save")) {
				JSON_OPEN("Resources/Data/Player/PlayerRevengeGage.json");
				JSON_OBJECT("PlayerRevengeGage");
				JSON_SAVE(addCoin_);
				JSON_SAVE(addGageBar_);
				JSON_SAVE(subGageBar_);
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
	isUpdate_ = true;
	isMove_ = true;
	currentRevengeBarGage_ = kMaxRevengeBar;
	Character::currentCharacterState_ = Character::State::kChase;
}

void PlayerRevengeGage::AddGage() {
	if (isMove_ || isUpdate_) {
		if (currentRevengeBarGage_ < kMaxRevengeBar) {
			currentRevengeBarGage_ += addCoin_;
		}
	}
	currentRevengeBarGage_ = std::clamp(currentRevengeBarGage_, 0.0f, kMaxRevengeBar);
}
