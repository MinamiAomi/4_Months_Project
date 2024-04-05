#include "PlayerUI.h"

#include "File/JsonHelper.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void PlayerUI::Initialize() {
	JSON_OPEN("Resources/Data/Player/PlayerUI.json");
	JSON_OBJECT("hpSpriteData_");
	JSON_LOAD(hpSpriteData_.position);
	JSON_LOAD(hpSpriteData_.scale);
	JSON_LOAD(hpSpriteData_.textureSize);
	JSON_LOAD(hpSpriteData_.textureBase);
	JSON_LOAD(hpSpriteData_.order);

	//JSON_ROOT();

	JSON_CLOSE();

	hpSprit_ = std::make_unique<Sprite>();
	hpSprit_->SetTexture(ResourceManager::GetInstance()->FindTexture("hp"));
	hpSprit_->SetIsActive(true);
	hpSprit_->SetDrawOrder(static_cast<uint8_t>(hpSpriteData_.order));
	hpSprit_->SetPosition(hpSpriteData_.position);
	hpSprit_->SetScale(hpSpriteData_.scale);
	hpSprit_->SetTexcoordBase(hpSpriteData_.textureBase);
	hpSprit_->SetTexcoordSize(hpSpriteData_.textureSize);
}

void PlayerUI::Update() {
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Player")) {
		if (ImGui::TreeNode("PlayerUI")) {
			
		}
	}
#endif // _DEBUG

}

void PlayerUI::DrawImGui(SpriteData& spriteData, std::string string, Sprite* sprite) {
	if (ImGui::TreeNode(string.c_str())) {
		ImGui::DragFloat3((string.c_str() + std::string(":position")).c_str(), &spriteData.position.x);
		ImGui::TreePop();
	}
	ImGui::TreePop();
}
