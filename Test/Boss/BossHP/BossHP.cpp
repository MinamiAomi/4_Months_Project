#include "BossHP.h"

#include "Graphics/ImGuiManager.h"

void BossHP::Initialize() {
	Reset();
}

void BossHP::Update() {
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Boss")) {
		if (ImGui::TreeNode("BossHP")) {
			int hp = int(currentHP_);
			ImGui::DragInt("HP", &hp,1,0, kMaxHP);
			currentHP_ = hp;
			ImGui::TreePop();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
}

void BossHP::Reset() {
	currentHP_ = kMaxHP;
}
