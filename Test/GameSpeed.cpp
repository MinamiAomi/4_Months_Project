#include "GameSpeed.h"

#include "Graphics/ImGuiManager.h"
#include "File/JsonHelper.h"
#include "CharacterState.h"

namespace GameSpeed {
	float chaseSpeed;
	float runAwaySpeed;
	float GetGameSpeed() {
		float speed = 0.0f;
		switch (characterState_) {
		case Character::State::kChase:
			speed = chaseSpeed;
		break;
		case Character::State::kRunAway:
			speed = runAwaySpeed;
		break;

		default:
			break;
		}
		return speed;
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
	}
}





