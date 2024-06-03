#include "TitleUI.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void TitleUI::Initialize() {
	LoadJson();
	pushA_ = CreateSprite(pushAData_, "pressA");
	titleLogo_ = CreateSprite(titleLogoData_, "titleLogo");
	pushAT_ = 0.0f;
}

void TitleUI::Update() {

	pushAT_ += 3.0f * Math::ToRadian;
	pushA_->SetPosition({ 960.0f, 230.0f + sinf(pushAT_) * 30.0f });
	
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("TitleUI")) {
		if (ImGui::TreeNode("pushA")) {

			DrawImGui(pushAData_, "controll", pushA_.get());
			DrawImGui(titleLogoData_, "titleLogo", titleLogo_.get());

			if (ImGui::Button("Save")) {
				SaveJson();
			}
			ImGui::TreePop();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG

}

void TitleUI::LoadJson() {
	JSON_OPEN("Resources/Data/TitleUI/TitleUI.json");
	JSON_OBJECT("pushAData_");
	pushAData_.Load();
	JSON_ROOT();
	JSON_OBJECT("titleLogoData_");
	titleLogoData_.Load();
	JSON_ROOT();
	JSON_CLOSE();
}

void TitleUI::SaveJson() {
	JSON_OPEN("Resources/Data/TitleUI/TitleUI.json");
	JSON_OBJECT("pushAData_");
	pushAData_.Save();
	JSON_ROOT();
	JSON_OBJECT("titleLogoData_");
	titleLogoData_.Save();
	JSON_ROOT();
	JSON_CLOSE();
}

std::unique_ptr<Sprite> TitleUI::CreateSprite(const SpriteData& spriteData, std::string spriteName) {
	auto sprite = std::make_unique<Sprite>();
	sprite->SetTexture(ResourceManager::GetInstance()->FindTexture(spriteName));
	sprite->SetIsActive(true);
	sprite->SetDrawOrder(static_cast<uint8_t>(spriteData.order));
	sprite->SetScale(spriteData.scale);
	sprite->SetRotate(spriteData.rotate);
	sprite->SetPosition(spriteData.position);
	sprite->SetColor(spriteData.color);
	sprite->SetTexcoordBase(spriteData.textureBase);
	sprite->SetTexcoordSize(spriteData.textureSize);
	return sprite;
}

void TitleUI::DrawImGui(SpriteData& spriteData, std::string string, Sprite* sprite) {
	sprite;
	spriteData;
#ifdef _DEBUG


	if (ImGui::TreeNode(string.c_str())) {
		ImGui::DragFloat2((string.c_str() + std::string(":scale")).c_str(), &spriteData.scale.x);
		ImGui::DragFloat((string.c_str() + std::string(":rotate")).c_str(), &spriteData.rotate, 0.1f);
		ImGui::DragFloat2((string.c_str() + std::string(":position")).c_str(), &spriteData.position.x);
		ImGui::DragFloat4((string.c_str() + std::string(":color")).c_str(), &spriteData.color.x, 0.1f, 0.0f, 1.0f);
		ImGui::DragFloat2((string.c_str() + std::string(":textureSize")).c_str(), &spriteData.textureSize.x);
		ImGui::DragFloat2((string.c_str() + std::string(":textureBase")).c_str(), &spriteData.textureBase.x);
		ImGui::DragInt((string.c_str() + std::string(":order")).c_str(), &spriteData.order);

		sprite->SetDrawOrder(static_cast<uint8_t>(spriteData.order));
		sprite->SetScale(spriteData.scale);
		sprite->SetRotate(spriteData.rotate);
		sprite->SetPosition(spriteData.position);
		sprite->SetColor(spriteData.color);
		sprite->SetTexcoordBase(spriteData.textureBase);
		sprite->SetTexcoordSize(spriteData.textureSize);


		ImGui::TreePop();
	}
#endif // _DEBUG
}

void TitleUI::SpriteData::Load() {
	JSON_LOAD(scale);
	JSON_LOAD(position);
	JSON_LOAD(rotate);
	JSON_LOAD(color);
	JSON_LOAD(textureSize);
	JSON_LOAD(textureBase);
	JSON_LOAD(order);
}

void TitleUI::SpriteData::Save() {
	JSON_SAVE(scale);
	JSON_SAVE(position);
	JSON_SAVE(rotate);
	JSON_SAVE(color);
	JSON_SAVE(textureSize);
	JSON_SAVE(textureBase);
	JSON_SAVE(order);
}
