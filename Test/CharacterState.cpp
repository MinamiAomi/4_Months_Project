#include "CharacterState.h"

#include "Graphics/ImGuiManager.h"
#include "File/JsonHelper.h"
#include "CharacterState.h"
namespace Character {
	State currentCharacterState_ = Character::kRunAway;
	State preCharacterState_ = currentCharacterState_;
	State nextCharacterState_ = currentCharacterState_;
	State saveCharacterState_;
	float time_;
	float toChaseTime_;
	float toRunAwayTime_;
	bool isEndFirstChange_ = false;
	bool isSignalChange_ = false;
	bool isChangingFirstFrame_ = false;
	bool isChangeEndFrame_ = false;

	float GetSceneChangeTime() {
		if (currentCharacterState_ == kScneChange) {
			float time = 0.0f;
			switch (nextCharacterState_) {
			case kChase:
				time = time_ / toChaseTime_;
				break;
			case kRunAway:
				time = time_ / toRunAwayTime_;
				break;
			default:
				break;
			}
			return time;
		}
		return 0.0f;
	}

	float GetSceneChangeT()
	{
		if (currentCharacterState_ == kScneChange) {
				float time = 0.0f;
				switch (nextCharacterState_) {
				case kChase:
					time = time_ / toChaseTime_;
					break;
				case kRunAway:
					time = 1.0f - time_ / toRunAwayTime_;
					break;
				default:
					break;
				}
				return time;
			
		}
		else if(currentCharacterState_ == kRunAway){
			return 0.0f;
		}
		else if (currentCharacterState_ == kChase) {
			return 1.0f;
		}

		return 0.0f;
	}

	void SetNextScene(const Character::State& scene) {
 		saveCharacterState_ = scene;
		isSignalChange_ = true;
	}

	bool IsInSceneChange() {
		return isChangingFirstFrame_;
	}

	bool IsOutSceneChange() {
		return isChangeEndFrame_;
	}

	void LoadJson() {
		JSON_OPEN("Resources/Data/Character/Character.json");
		JSON_OBJECT("GameSpeed");
		JSON_LOAD(toChaseTime_);
		JSON_LOAD(toRunAwayTime_);
		JSON_CLOSE();
	}

	void SaveJson() {
		JSON_OPEN("Resources/Data/Character/Character.json");
		JSON_OBJECT("GameSpeed");
		JSON_SAVE(toChaseTime_);
		JSON_SAVE(toRunAwayTime_);
		JSON_CLOSE();
	}

	void Update() {
		//一周回って
		if (isChangeEndFrame_) {
			isChangeEndFrame_ = false;
		}
		if (isChangingFirstFrame_ == true) {
			isChangingFirstFrame_ = false;
			time_ = 0.0f;
		}

		//signal 上に戻るまでtrueのフレーム
		if (isSignalChange_) {
			isSignalChange_ = false;
			isChangingFirstFrame_ = true;
			preCharacterState_ = currentCharacterState_;
			currentCharacterState_ = kScneChange;
			nextCharacterState_ = saveCharacterState_;
		}
		
		if (currentCharacterState_ == kScneChange) {
			time_ += 1.0f;
			float time = 0.0f;
			switch (nextCharacterState_) {
			case kChase:
				time = time_ / toChaseTime_;
				break;
			case kRunAway:
				time = time_ / toRunAwayTime_;
				break;
			default:
				break;
			}
			time = std::clamp(time, 0.0f, 1.0f);
			if (time >= 1.0f) {
				time_ = 0.0f;
				currentCharacterState_ = nextCharacterState_;
				isEndFirstChange_ = true;
				isChangeEndFrame_ = true;
			}
		}
#ifdef _DEBUG
		if (ImGui::BeginMenu("Character")) {
			ImGui::DragFloat("toChaseTime_", &toChaseTime_);
			ImGui::DragFloat("toRunAwayTime_", &toRunAwayTime_);
			if (ImGui::Button("Save")) {
				SaveJson();
			}
			ImGui::EndMenu();
		}
#endif // _DEBUG
	}
}