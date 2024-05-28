#include "BossHP.h"

#include "Graphics/ImGuiManager.h"
#include "File/JsonHelper.h"

void BossHP::Initialize() {
	Reset();
}

void BossHP::Update() {
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Boss")) {
		if (ImGui::TreeNode("BossHP")) {
			int hp = int(currentHP_);
			ImGui::DragInt("HP", &hp, 1, 0, kMaxHP);
			currentHP_ = hp;
			hp = ballHitDamage_;
			ImGui::DragInt("弾ダメージ量", &hp, 1, 0);
			ballHitDamage_ = hp;
			hp = playerHitDamage_;
			ImGui::DragInt("プレイヤーがヒット時ダメージ量", &hp, 1, 0);
			playerHitDamage_ = hp;
			if (ImGui::Button("Save")) {
				JSON_OPEN("Resources/Data/Boss/BossHP.json");
				JSON_OBJECT("BossHP");
				JSON_SAVE(ballHitDamage_);
				JSON_SAVE(playerHitDamage_);
				JSON_CLOSE();
			}
			ImGui::TreePop();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
}

void BossHP::Reset() {
	currentHP_ = kMaxHP;
	JSON_OPEN("Resources/Data/Boss/BossHP.json");
	JSON_OBJECT("BossHP");
	JSON_LOAD(ballHitDamage_);
	JSON_LOAD(playerHitDamage_);
	JSON_CLOSE();

}

void BossHP::AddPlayerHitHP() {
	currentHP_ -= playerHitDamage_;
	currentHP_ = std::clamp(currentHP_, 0, kMaxHP);

}

void BossHP::AddBallHitHP() {
	currentHP_ -= ballHitDamage_;
	currentHP_ = std::clamp(currentHP_, 0, kMaxHP);
}
