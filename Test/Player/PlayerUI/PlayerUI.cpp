#include "PlayerUI.h"


#include "File/JsonHelper.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void PlayerUI::Initialize() {
	LoadJson();
#pragma region HP
	for (uint32_t i = 0; auto & sprite : hpSprit_) {
		sprite = CreateSprite(hpSpriteData_.at(i), "hp");
		i++;
	}

	//hpBaseSprit_ = CreateSprite(hpBaseSpriteData_,"circle");
#pragma endregion
#pragma region 復讐ゲージ
	revengeBarGage_ = CreateSprite(revengeBarGageData_, "white2x2");

	revengeCircleGage_ = CreateSprite(revengeCircleGageData_, "circle");
#pragma endregion
}

void PlayerUI::Update() {
	HPUpdate();
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Player")) {
		if (ImGui::TreeNode("PlayerUI")) {

			DrawImGui(revengeBarGageData_, "revengeBarGageData_", revengeBarGage_.get());

			DrawImGui(revengeCircleGageData_, "revengeCircleGageData_", revengeCircleGage_.get());

			//DrawImGui(hpBaseSpriteData_,"hpBaseSpriteData_",hpBaseSprit_.get());

			for (uint32_t i = 0; i < hpSpriteData_.size(); i++) {
				std::string string = "hpSpriteData_.at(" + std::to_string(i) + ")";
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

void PlayerUI::HPUpdate() {
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

void PlayerUI::LoadJson() {
	JSON_OPEN("Resources/Data/Player/PlayerUI.json");
#pragma region HP

	JSON_OBJECT("hpSpriteData_");
	for (uint32_t i = 0; i < hpSpriteData_.size(); i++) {
		if (i == 0) {
			JSON_LOAD(hpSpriteData_.at(0).scale);
			JSON_LOAD(hpSpriteData_.at(0).position);
			JSON_LOAD(hpSpriteData_.at(0).rotate);
			JSON_LOAD(hpSpriteData_.at(0).color);
			JSON_LOAD(hpSpriteData_.at(0).textureSize);
			JSON_LOAD(hpSpriteData_.at(0).textureBase);
			JSON_LOAD(hpSpriteData_.at(0).order);
		}
		else if (i == 1) {
			JSON_LOAD(hpSpriteData_.at(1).scale);
			JSON_LOAD(hpSpriteData_.at(1).position);
			JSON_LOAD(hpSpriteData_.at(1).rotate);
			JSON_LOAD(hpSpriteData_.at(1).color);
			JSON_LOAD(hpSpriteData_.at(1).textureSize);
			JSON_LOAD(hpSpriteData_.at(1).textureBase);
			JSON_LOAD(hpSpriteData_.at(1).order);
		}
		else if (i == 2) {
			JSON_LOAD(hpSpriteData_.at(2).scale);
			JSON_LOAD(hpSpriteData_.at(2).position);
			JSON_LOAD(hpSpriteData_.at(2).rotate);
			JSON_LOAD(hpSpriteData_.at(2).color);
			JSON_LOAD(hpSpriteData_.at(2).textureSize);
			JSON_LOAD(hpSpriteData_.at(2).textureBase);
			JSON_LOAD(hpSpriteData_.at(2).order);
		}
	}

	JSON_ROOT();
	/*JSON_OBJECT("hpBaseSpriteData_");
	JSON_LOAD(hpBaseSpriteData_.scale);
	JSON_LOAD(hpBaseSpriteData_.position);
	JSON_LOAD(hpBaseSpriteData_.rotate);
	JSON_LOAD(hpBaseSpriteData_.color);
	JSON_LOAD(hpBaseSpriteData_.textureSize);
	JSON_LOAD(hpBaseSpriteData_.textureBase);
	JSON_LOAD(hpBaseSpriteData_.order);

	JSON_ROOT();*/

#pragma endregion
#pragma region 復讐ゲージ
	// 棒
	JSON_OBJECT("revengeBarGageData_");
	JSON_LOAD(revengeBarGageData_.scale);
	JSON_LOAD(revengeBarGageData_.position);
	JSON_LOAD(revengeBarGageData_.rotate);
	JSON_LOAD(revengeBarGageData_.color);
	JSON_LOAD(revengeBarGageData_.textureSize);
	JSON_LOAD(revengeBarGageData_.textureBase);
	JSON_LOAD(revengeBarGageData_.order);

	JSON_ROOT();
	// 円
	JSON_OBJECT("revengeCircleGageData_");
	JSON_LOAD(revengeCircleGageData_.scale);
	JSON_LOAD(revengeCircleGageData_.position);
	JSON_LOAD(revengeCircleGageData_.rotate);
	JSON_LOAD(revengeCircleGageData_.color);
	JSON_LOAD(revengeCircleGageData_.textureSize);
	JSON_LOAD(revengeCircleGageData_.textureBase);
	JSON_LOAD(revengeCircleGageData_.order);

	JSON_ROOT();
#pragma endregion

	JSON_CLOSE();

}

void PlayerUI::SaveJson() {
	JSON_OPEN("Resources/Data/Player/PlayerUI.json");

#pragma region HP
	JSON_OBJECT("hpSpriteData_");
	for (uint32_t i = 0; i < hpSpriteData_.size(); i++) {
		if (i == 0) {
			JSON_LOAD(hpSpriteData_.at(0).scale);
			JSON_LOAD(hpSpriteData_.at(0).position);
			JSON_LOAD(hpSpriteData_.at(0).rotate);
			JSON_LOAD(hpSpriteData_.at(0).color);
			JSON_LOAD(hpSpriteData_.at(0).textureSize);
			JSON_LOAD(hpSpriteData_.at(0).textureBase);
			JSON_LOAD(hpSpriteData_.at(0).order);
		}
		else if (i == 1) {
			JSON_LOAD(hpSpriteData_.at(1).scale);
			JSON_LOAD(hpSpriteData_.at(1).position);
			JSON_LOAD(hpSpriteData_.at(1).rotate);
			JSON_LOAD(hpSpriteData_.at(1).color);
			JSON_LOAD(hpSpriteData_.at(1).textureSize);
			JSON_LOAD(hpSpriteData_.at(1).textureBase);
			JSON_LOAD(hpSpriteData_.at(1).order);
		}
		else if (i == 2) {
			JSON_LOAD(hpSpriteData_.at(2).scale);
			JSON_LOAD(hpSpriteData_.at(2).position);
			JSON_LOAD(hpSpriteData_.at(2).rotate);
			JSON_LOAD(hpSpriteData_.at(2).color);
			JSON_LOAD(hpSpriteData_.at(2).textureSize);
			JSON_LOAD(hpSpriteData_.at(2).textureBase);
			JSON_LOAD(hpSpriteData_.at(2).order);
		}
	}
	JSON_ROOT();

	/*JSON_OBJECT("hpBaseSpriteData_");
	JSON_SAVE(hpBaseSpriteData_.scale);
	JSON_SAVE(hpBaseSpriteData_.position);
	JSON_SAVE(hpBaseSpriteData_.rotate);
	JSON_SAVE(hpBaseSpriteData_.color);
	JSON_SAVE(hpBaseSpriteData_.textureSize);
	JSON_SAVE(hpBaseSpriteData_.textureBase);
	JSON_SAVE(hpBaseSpriteData_.order);

	JSON_ROOT();*/
#pragma endregion
#pragma region 復讐ゲージ
	// 棒
	JSON_OBJECT("revengeBarGageData_");
	JSON_SAVE(revengeBarGageData_.scale);
	JSON_SAVE(revengeBarGageData_.position);
	JSON_SAVE(revengeBarGageData_.rotate);
	JSON_SAVE(revengeBarGageData_.color);
	JSON_SAVE(revengeBarGageData_.textureSize);
	JSON_SAVE(revengeBarGageData_.textureBase);
	JSON_SAVE(revengeBarGageData_.order);

	JSON_ROOT();
	// 円
	JSON_OBJECT("revengeCircleGageData_");
	JSON_SAVE(revengeCircleGageData_.scale);
	JSON_SAVE(revengeCircleGageData_.position);
	JSON_SAVE(revengeCircleGageData_.rotate);
	JSON_SAVE(revengeCircleGageData_.color);
	JSON_SAVE(revengeCircleGageData_.textureSize);
	JSON_SAVE(revengeCircleGageData_.textureBase);
	JSON_SAVE(revengeCircleGageData_.order);

	JSON_ROOT();
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
}