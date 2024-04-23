#include "PlayerUI.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void PlayerUI::Initialize() {
	LoadJson();
#pragma region HP
	for (uint32_t i = 0; auto & sprite : hpSprit_) {
		sprite = CreateSprite(hpSpriteData_.at(i), "HPBar_Gauge");
		i++;
	}

	hpBaseSprit_ = CreateSprite(hpBaseSpriteData_, "HPBar_Flame");
#pragma endregion
#pragma region 復讐ゲージ
	revengeBarGage_ = CreateSprite(revengeBarGageData_, "RevengeBar_RevengeGauge");
	revengeBarGageBase_ = CreateSprite(revengeBarGageBaseData_, "RevengeBar_Flame");

	revengeCircleGage_ = CreateSprite(revengeCircleGageData_, "RevengeBar_WeaponGauge");
	//revengeCircleGageBase_ = CreateSprite(revengeCircleGageBaseData_, "circle");
#pragma endregion
}

void PlayerUI::Update() {
	UpdateHP();
	UpdateRevengeGage();
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Player")) {
		if (ImGui::TreeNode("PlayerUI")) {

			DrawImGui(revengeBarGageData_, "revengeBarGageData_", revengeBarGage_.get());
			DrawImGui(revengeBarGageBaseData_, "revengeBarGageBaseData_", revengeBarGageBase_.get());

			DrawImGui(revengeCircleGageData_, "revengeCircleGageData_", revengeCircleGage_.get());
			//DrawImGui(revengeCircleGageBaseData_, "revengeCircleGageBaseData_", revengeCircleGageBase_.get());

			DrawImGui(hpBaseSpriteData_, "hpBaseSpriteData_", hpBaseSprit_.get());

			for (uint32_t i = 0; i < hpSpriteData_.size(); i++) {
				std::string string = "hpSpriteData:(" + std::to_string(i) + ")";
				DrawImGui(hpSpriteData_.at(i), string, hpSprit_.at(i).get());
			}

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

void PlayerUI::UpdateHP() {
	if (playerHP_->GetCurrentHP() == 0) {
		hpSprit_.at(0)->SetIsActive(false);
		hpSprit_.at(1)->SetIsActive(false);
		hpSprit_.at(2)->SetIsActive(false);
	}
	else if (playerHP_->GetCurrentHP() == 1) {
		hpSprit_.at(0)->SetIsActive(false);
		hpSprit_.at(1)->SetIsActive(false);
		hpSprit_.at(2)->SetIsActive(true);
	}
	else if (playerHP_->GetCurrentHP() == 2) {
		hpSprit_.at(0)->SetIsActive(false);
		hpSprit_.at(1)->SetIsActive(true);
		hpSprit_.at(2)->SetIsActive(true);
	}
	else if (playerHP_->GetCurrentHP() == 3) {
		hpSprit_.at(0)->SetIsActive(true);
		hpSprit_.at(1)->SetIsActive(true);
		hpSprit_.at(2)->SetIsActive(true);
	}
}

void PlayerUI::UpdateRevengeGage() {
	float barT = playerRevengeGage_->GetCurrentRevengeBarGage() / PlayerRevengeGage::kMaxRevengeBar;
	float circleT = playerRevengeGage_->GetCurrentRevengeCircleGage() / PlayerRevengeGage::kMaxRevengeCircle;
#pragma region Bar
	revengeBarGage_->SetPosition(
		{
		revengeBarGageData_.position.x,
		std::lerp(revengeBarGageData_.position.y - revengeBarGageData_.scale.y * 0.5f, revengeBarGageData_.position.y,barT)
		}
	);
	revengeBarGage_->SetScale(
		{
		revengeBarGageData_.scale.x,
		std::lerp(0.0f, revengeBarGageData_.scale.y,barT)
		}
	);
	revengeBarGage_->SetTexcoordSize(
		{
			revengeBarGageData_.textureSize.x,
			std::lerp(0.0f, revengeBarGageData_.textureSize.y,barT)
		}
	);
	revengeBarGage_->SetTexcoordBase(
		{
			revengeBarGageData_.textureSize.x,
			std::lerp(0.0f, -revengeBarGageData_.textureSize.y,barT)
		}
	);
#pragma endregion

	revengeCircleGage_->SetScale(
		{
		std::lerp(0.0f, revengeCircleGageData_.scale.x,circleT),
		std::lerp(0.0f, revengeCircleGageData_.scale.y,circleT)
		}
	);

}

void PlayerUI::LoadJson() {
	JSON_OPEN("Resources/Data/Player/PlayerUI.json");
#pragma region HP

	for (uint32_t i = 0; i < hpSpriteData_.size(); i++) {
		JSON_OBJECT("hpSpriteData_" + std::to_string(i));
		hpSpriteData_.at(i).Load();
		JSON_ROOT();
	}

	JSON_ROOT();

	JSON_OBJECT("hpBaseSpriteData_");
	hpBaseSpriteData_.Load();
	JSON_ROOT();

#pragma endregion
#pragma region 復讐ゲージ
	// 棒
	JSON_OBJECT("revengeBarGageData_");
	revengeBarGageData_.Load();
	JSON_ROOT();
	// Base
	JSON_OBJECT("revengeBarGageBaseData_");
	revengeBarGageBaseData_.Load();
	JSON_ROOT();

	// 円
	JSON_OBJECT("revengeCircleGageData_");
	revengeCircleGageData_.Load();
	JSON_ROOT();
	/*JSON_OBJECT("revengeCircleGageBaseData_");
	revengeCircleGageBaseData_.Load();
	JSON_ROOT();*/
#pragma endregion
	JSON_CLOSE();

}

void PlayerUI::SaveJson() {
	JSON_OPEN("Resources/Data/Player/PlayerUI.json");
#pragma region HP
	for (uint32_t i = 0; i < hpSpriteData_.size(); i++) {
		JSON_OBJECT("hpSpriteData_" + std::to_string(i));
		hpSpriteData_.at(i).Save();
		JSON_ROOT();
	}
	JSON_ROOT();
	JSON_OBJECT("hpBaseSpriteData_");
	hpBaseSpriteData_.Save();
	JSON_ROOT();
#pragma endregion
#pragma region 復讐ゲージ
	// 棒
	JSON_OBJECT("revengeBarGageData_");
	revengeBarGageData_.Save();
	JSON_ROOT();
	// 棒
	JSON_OBJECT("revengeBarGageBaseData_");
	revengeBarGageBaseData_.Save();
	JSON_ROOT();
	// 円
	JSON_OBJECT("revengeCircleGageData_");
	revengeCircleGageData_.Save();
	JSON_ROOT();
	// 円
	/*JSON_OBJECT("revengeCircleGageBaseData_");
	revengeCircleGageBaseData_.Save();
	JSON_ROOT();*/
#pragma endregion
	JSON_CLOSE();
}

std::unique_ptr<Sprite> PlayerUI::CreateSprite(const SpriteData& spriteData, std::string spriteName) {
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

void PlayerUI::DrawImGui(SpriteData& spriteData, std::string string, Sprite* sprite) {
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

void PlayerUI::SpriteData::Load() {
	JSON_LOAD(scale);
	JSON_LOAD(position);
	JSON_LOAD(rotate);
	JSON_LOAD(color);
	JSON_LOAD(textureSize);
	JSON_LOAD(textureBase);
	JSON_LOAD(order);
}

void PlayerUI::SpriteData::Save() {
	JSON_SAVE(scale);
	JSON_SAVE(position);
	JSON_SAVE(rotate);
	JSON_SAVE(color);
	JSON_SAVE(textureSize);
	JSON_SAVE(textureBase);
	JSON_SAVE(order);
}
