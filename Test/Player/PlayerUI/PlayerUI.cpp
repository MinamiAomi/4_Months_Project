#include "PlayerUI.h"


#include "File/JsonHelper.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void PlayerUI::Initialize() {
	JSON_OPEN("Resources/Data/Player/PlayerUI.json");
	JSON_OBJECT("hpBaseSpriteData_");
	JSON_LOAD(hpBaseSpriteData_.scale);
	JSON_LOAD(hpBaseSpriteData_.position);
	JSON_LOAD(hpBaseSpriteData_.rotate);
	JSON_LOAD(hpBaseSpriteData_.color);
	JSON_LOAD(hpBaseSpriteData_.textureSize);
	JSON_LOAD(hpBaseSpriteData_.textureBase);
	JSON_LOAD(hpBaseSpriteData_.order);

	JSON_ROOT();
	
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

	//JSON_ROOT();

	JSON_CLOSE();

	for (uint32_t i = 0; auto & sprite : hpSprit_) {
		sprite = std::make_unique<Sprite>();
		sprite->SetTexture(ResourceManager::GetInstance()->FindTexture("hp"));
		sprite->SetIsActive(true);
		sprite->SetDrawOrder(static_cast<uint8_t>(hpSpriteData_.at(i).order));
		sprite->SetScale(hpSpriteData_.at(i).scale);
		sprite->SetRotate(hpSpriteData_.at(i).rotate);
		sprite->SetPosition(hpSpriteData_.at(i).position);
		sprite->SetColor(hpSpriteData_.at(i).color);
		sprite->SetTexcoordBase(hpSpriteData_.at(i).textureBase);
		sprite->SetTexcoordSize(hpSpriteData_.at(i).textureSize);
		i++;
	}

	hpBaseSprit_ = std::make_unique<Sprite>();
	hpBaseSprit_->SetTexture(ResourceManager::GetInstance()->FindTexture("circle"));
	hpBaseSprit_->SetIsActive(true);
	hpBaseSprit_->SetDrawOrder(static_cast<uint8_t>(hpBaseSpriteData_.order));
	hpBaseSprit_->SetScale(hpBaseSpriteData_.scale);
	hpBaseSprit_->SetRotate(hpBaseSpriteData_.rotate);
	hpBaseSprit_->SetPosition(hpBaseSpriteData_.position);
	hpBaseSprit_->SetColor(hpBaseSpriteData_.color);
	hpBaseSprit_->SetTexcoordBase(hpBaseSpriteData_.textureBase);
	hpBaseSprit_->SetTexcoordSize(hpBaseSpriteData_.textureSize);
}

void PlayerUI::Update() {
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Player")) {
		if (ImGui::TreeNode("PlayerUI")) {

			DrawImGui(hpBaseSpriteData_,"hpBaseSpriteData_",hpBaseSprit_.get());

			for (uint32_t i = 0; i < hpSpriteData_.size(); i++) {
				std::string string = "hpSpriteData_.at(" + std::to_string(i) + ")";
				DrawImGui(hpSpriteData_.at(i), string, hpSprit_.at(i).get());
			}

			if (ImGui::Button("Save")) {
				JSON_OPEN("Resources/Data/Player/PlayerUI.json");

				JSON_OBJECT("hpBaseSpriteData_");
				JSON_SAVE(hpBaseSpriteData_.scale);
				JSON_SAVE(hpBaseSpriteData_.position);
				JSON_SAVE(hpBaseSpriteData_.rotate);
				JSON_SAVE(hpBaseSpriteData_.color);
				JSON_SAVE(hpBaseSpriteData_.textureSize);
				JSON_SAVE(hpBaseSpriteData_.textureBase);
				JSON_SAVE(hpBaseSpriteData_.order);

				JSON_ROOT();

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
				//JSON_ROOT();

				JSON_CLOSE();
			}
			ImGui::TreePop();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG

}

void PlayerUI::DrawImGui(SpriteData& spriteData, std::string string, Sprite* sprite) {
	if (ImGui::TreeNode(string.c_str())) {
		ImGui::DragFloat2((string.c_str() + std::string(":scale")).c_str(), &spriteData.scale.x);
		ImGui::DragFloat((string.c_str() + std::string(":rotate")).c_str(), &spriteData.rotate, 0.1f);
		ImGui::DragFloat2((string.c_str() + std::string(":position")).c_str(), &spriteData.position.x);
		ImGui::DragFloat4((string.c_str() + std::string(":color")).c_str(), &spriteData.color.x);
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