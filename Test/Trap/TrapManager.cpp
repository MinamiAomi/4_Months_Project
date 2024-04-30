#include "TrapManager.h"

#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"

void TrapManager::Initialize() {
	JSON_OPEN("Resources/Data/Trap/Trap.json");
	JSON_OBJECT("Trap");
	JSON_LOAD(velocity_);
	JSON_CLOSE();
	Reset();
}

void TrapManager::Update() {
	for (auto& trap : traps_) {
		trap->Update();
	}
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Trap")) {
		ImGui::DragFloat("velocity_", &velocity_, 0.1f);
		if (ImGui::Button("Save")) {
			JSON_OPEN("Resources/Data/Trap/Trap.json");
			JSON_OBJECT("Trap");
			JSON_SAVE(velocity_);
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG

}

void TrapManager::Create(const Vector3& position) {
	Trap* trap = new Trap();
	Trap::Desc desc{};
	desc.pos = position;
	desc.velocity = velocity_;
	trap->SetCamera(camera_);
	trap->Initialize(desc);
	traps_.emplace_back(std::move(trap));
}

void TrapManager::Reset() {
	traps_.clear();
}
