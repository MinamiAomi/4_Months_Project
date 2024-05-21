#include "BossUI.h"

#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void BossUI::Initialize() {
	LoadJson();
	hp_ = CreateSprite(hpData_, "white2x2");
	hpBase_ = CreateSprite(hpBaseData_, "white2x2");
}

void BossUI::Update() {
	UpdateHP();
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Boss")) {
		if (ImGui::TreeNode("BossUI")) {
			DrawImGui(hpData_, "hpData_", hp_.get());
			DrawImGui(hpBaseData_, "hpBaseData_", hpBase_.get());
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

void BossUI::UpdateHP() {
	float currentHP = float(bossHP_->GetCurrentHP());
	float barT = currentHP / float(BossHP::kMaxHP);
	hp_->SetPosition(
		{
		std::lerp(hpData_.position.x - hpData_.scale.x * 0.5f, hpData_.position.x,barT),
		hpData_.position.y
		}
	);
	hp_->SetScale(
		{
		std::lerp(0.0f, hpData_.scale.x,barT),
		hpData_.scale.y,
		}
	);
	hp_->SetTexcoordSize(
		{
			std::lerp(0.0f, hpData_.textureSize.x,barT),
			hpData_.textureSize.y
		}
	);
	hp_->SetTexcoordBase(
		{
			std::lerp(0.0f, -hpData_.textureSize.x,barT),
			hpData_.textureSize.y,
		}
	);
}

void BossUI::LoadJson() {
	JSON_OPEN("Resources/Data/Boss/BossUI.json");
	JSON_OBJECT("hpData_");
	hpData_.Load();
	JSON_ROOT();
	JSON_CLOSE();
}

void BossUI::SaveJson() {
	JSON_OPEN("Resources/Data/Boss/BossUI.json");
	JSON_OBJECT("hpData_");
	hpData_.Save();
	JSON_ROOT();
	JSON_CLOSE();
}

std::unique_ptr<Sprite> BossUI::CreateSprite(const SpriteData& spriteData, std::string spriteName) {
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

void BossUI::DrawImGui(SpriteData& spriteData, std::string string, Sprite* sprite) {
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


void BossUI::SpriteData::Load() {
	JSON_LOAD(scale);
	JSON_LOAD(position);
	JSON_LOAD(rotate);
	JSON_LOAD(color);
	JSON_LOAD(textureSize);
	JSON_LOAD(textureBase);
	JSON_LOAD(order);
}

void BossUI::SpriteData::Save() {
	JSON_SAVE(scale);
	JSON_SAVE(position);
	JSON_SAVE(rotate);
	JSON_SAVE(color);
	JSON_SAVE(textureSize);
	JSON_SAVE(textureBase);
	JSON_SAVE(order);
}
