#include "CharacterState.h"

#include "Graphics/ImGuiManager.h"
#include "File/JsonHelper.h"
#include "CharacterState.h"
namespace Character {
	State currentCharacterState_ = Character::kRunAway;
	State preCharacterState_ = currentCharacterState_;
	State nextCharacterState_ = currentCharacterState_;
	float time_;
	float toChaseTime_;
	float toRunAwayTime_;
	bool isEndFirstChange_;

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
		currentCharacterState_ = kScneChange;
		nextCharacterState_ = scene;
		time_ = 0.0f;
	}

	bool IsInSceneChange() {
		return currentCharacterState_ == kScneChange && preCharacterState_ != kScneChange;
	}

	bool IsOutSceneChange() {
		return currentCharacterState_ != kScneChange && preCharacterState_ == kScneChange;
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
		preCharacterState_ = currentCharacterState_;
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