#include "UI.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Pause.h"

void UI::Initialize() {
	LoadJson();
	controll_ = CreateSprite(controllData_, "ControllUI");
}

void UI::Update() {
	
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("UI")) {
		if (ImGui::TreeNode("controll")) {

			DrawImGui(controllData_, "controll", controll_.get());

			if (ImGui::Button("Save")) {
				SaveJson();
			}
			ImGui::TreePop();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG

	controll_->SetIsActive(false);
}

void UI::LoadJson() {
	JSON_OPEN("Resources/Data/UI/UI.json");
	JSON_OBJECT("controllData_");
	controllData_.Load();
	JSON_CLOSE();
}

void UI::SaveJson() {
	JSON_OPEN("Resources/Data/UI/UI.json");
	JSON_OBJECT("controllData_");
	controllData_.Save();
	JSON_CLOSE();
}

std::unique_ptr<Sprite> UI::CreateSprite(const SpriteData& spriteData, std::string spriteName) {
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

void UI::DrawImGui(SpriteData& spriteData, std::string string, Sprite* sprite) {
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

void UI::SpriteData::Load() {
	JSON_LOAD(scale);
	JSON_LOAD(position);
	JSON_LOAD(rotate);
	JSON_LOAD(color);
	JSON_LOAD(textureSize);
	JSON_LOAD(textureBase);
	JSON_LOAD(order);
}

void UI::SpriteData::Save() {
	JSON_SAVE(scale);
	JSON_SAVE(position);
	JSON_SAVE(rotate);
	JSON_SAVE(color);
	JSON_SAVE(textureSize);
	JSON_SAVE(textureBase);
	JSON_SAVE(order);
}
