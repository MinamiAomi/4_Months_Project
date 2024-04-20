#include "GameSpeed.h"

#include "Graphics/ImGuiManager.h"
#include "File/JsonHelper.h"
namespace GameSpeed {
	float speed;
	void LoadJson() {
		JSON_OPEN("Resources/Data/GameSpeed/GameSpeed.json");
		JSON_OBJECT("GameSpeed");
		JSON_LOAD(speed);
		JSON_CLOSE();
	}
	void SaveJson() {
		JSON_OPEN("Resources/Data/GameSpeed/GameSpeed.json");
		JSON_OBJECT("GameSpeed");
		JSON_SAVE(speed);
		JSON_CLOSE();
	}
	void Update() {
#ifdef _DEBUG
		if (ImGui::BeginMenu("GameSpeed")) {
			ImGui::DragFloat("speed", &speed, 0.1f);
			if (ImGui::Button("Save")) {
				SaveJson();
			}
			ImGui::EndMenu();
		}
#endif // _DEBUG
	}
}





