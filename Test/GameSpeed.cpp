#include "GameSpeed.h"

#include "Graphics/ImGuiManager.h"
#include "File/JsonHelper.h"
#include "CharacterState.h"
#include "Movie.h"

namespace GameSpeed {
	float chaseSpeed;
	float runAwaySpeed;
	float currentSpeed = 0.0f;
	float GetGameSpeed() {
		return currentSpeed;
	}
	void LoadJson() {
		JSON_OPEN("Resources/Data/GameSpeed/GameSpeed.json");
		JSON_OBJECT("GameSpeed");
		JSON_LOAD(chaseSpeed);
		JSON_LOAD(runAwaySpeed);
		JSON_CLOSE();
	}
	void SaveJson() {
		JSON_OPEN("Resources/Data/GameSpeed/GameSpeed.json");
		JSON_OBJECT("GameSpeed");
		JSON_SAVE(chaseSpeed);
		JSON_SAVE(runAwaySpeed);
		JSON_CLOSE();
	}
	void Update() {
#ifdef _DEBUG
		if (ImGui::BeginMenu("GameSpeed")) {
			ImGui::DragFloat("chaseSpeed", &chaseSpeed, 0.1f);
			ImGui::DragFloat("runAwaySpeed", &runAwaySpeed, 0.1f);
			if (ImGui::Button("Save")) {
				SaveJson();
			}
			ImGui::EndMenu();
		}
#endif // _DEBUG

		//currentSpeedにspeedを代入
		if (Movie::isPlaying) {
			currentSpeed = 0.0f;
		}
		else {
			switch (Character::currentCharacterState_) {
			case Character::State::kChase:
				currentSpeed = chaseSpeed;
				break;
			case Character::State::kRunAway:
				currentSpeed = runAwaySpeed;
				break;
			default:
				break;
			}
		}
	}
}





