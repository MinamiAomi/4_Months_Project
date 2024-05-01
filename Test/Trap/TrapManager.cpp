#include "TrapManager.h"

#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"
#include "Player/Player.h"

void TrapManager::Initialize() {
	JSON_OPEN("Resources/Data/Trap/Trap.json");
	JSON_OBJECT("Trap");
	JSON_LOAD(desc_.shotVelocity);
	JSON_LOAD(desc_.dropVelocity);
	JSON_LOAD(desc_.offset);
	JSON_CLOSE();
	Reset();
}

void TrapManager::Update() {
	bool isDrop = false;
	for (auto it = traps_.begin(); it != traps_.end();) {
		(*it)->Update();
		// 落下中のTrapを探す
		if ((*it)->GetState() == Trap::State::kDrop) {
			isDrop = true;
		}
		if (!(*it)->GetIsAlive()) {
			it = traps_.erase(it);
		}
		else {
			++it;
		}
	}
	// １つも落下中で無かったら
	if (!isDrop&& 
		Character::currentCharacterState_ == Character::State::kRunAway) {
		Create(player_->transform.worldMatrix.GetTranslate());
	}
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Trap")) {
		ImGui::DragFloat("shotVelocity", &desc_.shotVelocity, 0.1f);
		ImGui::DragFloat("dropVelocity", &desc_.dropVelocity, 0.1f);
		ImGui::DragFloat("offset", &desc_.offset, 0.1f);
		if (ImGui::Button("Save")) {
			JSON_OPEN("Resources/Data/Trap/Trap.json");
			JSON_OBJECT("Trap");
			JSON_SAVE(desc_.shotVelocity);
			JSON_SAVE(desc_.dropVelocity);
			JSON_SAVE(desc_.offset);
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
	desc = desc_;
	desc.pos = position;
	trap->SetCamera(camera_);
	trap->SetPlayer(player_);
	trap->Initialize(desc);
	traps_.emplace_back(std::move(trap));
}

void TrapManager::Reset() {
	traps_.clear();
}
