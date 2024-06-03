#include "PlayerUI.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Math/MathUtils.h"
#include"Engine/Graphics/GameWindow.h"
#include "Player/Player.h"
#include "Math/Camera.h"

void PlayerUI::Initialize() {
	LoadJson();

	//拡大比率統一
	playerFrameSpriteData_.scale = (Vector2(playerFrameSpriteData_.textureSize.x * userFrameSpriteRatioUI, playerFrameSpriteData_.textureSize.y * userFrameSpriteRatioUI));
	toBossDistanceBarSpriteData_.scale = (Vector2(toBossDistanceBarSpriteData_.textureSize.x * ratioUI, toBossDistanceBarSpriteData_.textureSize.y * ratioUI));
	toBossDistanceMeterSpriteData_.scale = (Vector2(toBossDistanceMeterSpriteData_.textureSize.x * ratioUI, toBossDistanceMeterSpriteData_.textureSize.y * ratioUI));
	toBossDistanceNumberSpriteData_.scale = (Vector2(toBossDistanceNumberSpriteData_.textureSize.x * ratioUI, toBossDistanceNumberSpriteData_.textureSize.y * ratioUI));
	revengeBarGaugeSpriteData_.scale = (Vector2(revengeBarGaugeSpriteData_.textureSize.x * ratioUI, revengeBarGaugeSpriteData_.textureSize.y * ratioUI));
	revengeBarGaugeBaseSpriteData_.scale = (Vector2(revengeBarGaugeBaseSpriteData_.textureSize.x * ratioUI, revengeBarGaugeBaseSpriteData_.textureSize.y * ratioUI));
	revengeBarIconSpriteData_.scale = (Vector2(revengeBarIconSpriteData_.textureSize.x * ratioUI, revengeBarIconSpriteData_.textureSize.y * ratioUI));
	for (uint32_t i = 0; auto & spriteData : hpSpriteData_) {
		spriteData.scale = (Vector2(spriteData.textureSize.x * ratioUI, spriteData.textureSize.y * ratioUI));
		i++;
	}

#pragma region 全体フレーム

	userFrameSprite_ = CreateSprite(userFrameSpriteData_, "FrameUI");
	userFrameSprite2_ = CreateSprite(userFrameSpriteData2_, "FrameUI_2");
	userFrameSprite_->SetIsActive(false);

#pragma endregion

#pragma region プレイヤーUIフレーム

	playerFrameSprite_ = CreateSprite(playerFrameSpriteData_, "PlayerUIFrame");

#pragma endregion

#pragma region ボスとの距離

	toBossDistanceBarSprite_ = CreateSprite(toBossDistanceBarSpriteData_, "ToBossDistanceBar");
	toBossDistanceMeterSprite_ = CreateSprite(toBossDistanceMeterSpriteData_, "ToBossDistanceMeter");
	toBossDistanceNumberSprite_ = CreateSprite(toBossDistanceNumberSpriteData_, "ToBossDistanceNumber");

#pragma endregion

#pragma region HP
	for (uint32_t i = 0; auto & sprite : hpSprite_) {
		sprite = CreateSprite(hpSpriteData_.at(i), "HPBar_Gauge");
		i++;
	}

#pragma endregion
#pragma region 復讐ゲージ
	revengeBarGaugeSprite_ = CreateSprite(revengeBarGaugeSpriteData_, "RevengeBar_RevengeGauge");
	revengeBarGaugeBaseSprite_ = CreateSprite(revengeBarGaugeBaseSpriteData_, "RevengeBar_Frame");
	revengeBarIconSprite_ = CreateSprite(revengeBarIconSpriteData_, "RevengeBar_Icon");

	tutorial1_ = CreateSprite(tutorial1Data_, "tutorial1");
	tutorial2_ = CreateSprite(tutorial2Data_, "tutorial2");
	tutorial3_ = CreateSprite(tutorial3Data_, "tutorial3");

#pragma endregion

	tutorial1_->SetIsActive(false);
	tutorial2_->SetIsActive(false);
	tutorial3_->SetIsActive(false);

}

void PlayerUI::Update() {
	UpdatePlayerUI();
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Player")) {
		if (ImGui::TreeNode("PlayerUI")) {

			DrawImGui(userFrameSpriteData_, "userFrameSpriteData_", userFrameSprite_.get());

			DrawImGui(playerFrameSpriteData_, "playerFrameSpriteData_", playerFrameSprite_.get());

			DrawImGui(toBossDistanceBarSpriteData_, "toBossDistanceBarSpriteData_", toBossDistanceBarSprite_.get());

			DrawImGui(toBossDistanceMeterSpriteData_, "toBossDistanceMeterSpriteData_", toBossDistanceMeterSprite_.get());

			DrawImGui(toBossDistanceNumberSpriteData_, "toBossDistanceNumberSpriteData_", toBossDistanceNumberSprite_.get());

			DrawImGui(revengeBarGaugeSpriteData_, "revengeBarGageSpriteData_", revengeBarGaugeSprite_.get());

			DrawImGui(revengeBarGaugeBaseSpriteData_, "revengeBarGageBaseSpriteData_", revengeBarGaugeBaseSprite_.get());

			DrawImGui(revengeBarIconSpriteData_, "revengeBarIconSpriteData_", revengeBarIconSprite_.get());

			DrawImGui(tutorial1Data_, "tutorial1Data_", tutorial1_.get());

			DrawImGui(tutorial2Data_, "tutorial2Data_", tutorial2_.get());

			DrawImGui(tutorial3Data_, "tutorial3Data_", tutorial3_.get());

			for (uint32_t i = 0; i < hpSpriteData_.size(); i++) {
				std::string string = "hpSpriteData:(" + std::to_string(i) + ")";
				DrawImGui(hpSpriteData_.at(i), string, hpSprite_.at(i).get());
			}

			ImGui::DragFloat("UI比率", &ratioUI, 0.01f, 0.0f,100.0f);

			if (ImGui::Button("Save")) {
				SaveJson();
			}
			ImGui::TreePop();
		}
		ImGui::EndMenu();
	}
	ImGui::End();


#endif // _DEBUG

	RECT rect;
	GetWindowRect(GameWindow::GetInstance()->GetHWND(), &rect);
	LONG width = rect.right - rect.left;
	LONG height = rect.bottom - rect.top;

	Matrix4x4 vpv = camera_->GetViewMatrix() * camera_->GetProjectionMatrix() ;
	vpv *= Matrix4x4::MakeScaling({ 1.0f, -1.0f, 1.0f }) * Matrix4x4::MakeTranslation({0.0f, 0.0f, 0.0f});
	vpv *= Matrix4x4::MakeViewport(0, 0, float(width), float(height), camera_->GetNearClip(), camera_->GetFarClip());
	Vector3 viewportPlayerPosition = vpv.ApplyTransformWDivide(player_->transform.worldMatrix.GetTranslate());

	playerFrameSprite_->SetPosition(Vector2(viewportPlayerPosition.x - 20.0f, viewportPlayerPosition.y - 20.0f));

	//プレイヤーUIフレームを親にする
	UpdateRevengeGage();
	//revengeBarGaugeSprite_->SetPosition(playerFrameSprite_->GetPosition() + revengeBarGaugeSpriteData_.position);
	revengeBarGaugeBaseSprite_->SetPosition(playerFrameSprite_->GetPosition() + revengeBarGaugeBaseSpriteData_.position);
	revengeBarIconSprite_->SetPosition(playerFrameSprite_->GetPosition() + revengeBarIconSpriteData_.position);
	toBossDistanceBarSprite_->SetPosition(playerFrameSprite_->GetPosition() + toBossDistanceBarSpriteData_.position);
	toBossDistanceMeterSprite_->SetPosition(playerFrameSprite_->GetPosition() + toBossDistanceMeterSpriteData_.position);
	toBossDistanceNumberSprite_->SetPosition(playerFrameSprite_->GetPosition() + toBossDistanceNumberSpriteData_.position);

	hpSprite_.at(0)->SetPosition(playerFrameSprite_->GetPosition() + hpSpriteData_.at(0).position);
	hpSprite_.at(1)->SetPosition(playerFrameSprite_->GetPosition() + hpSpriteData_.at(1).position);
	hpSprite_.at(2)->SetPosition(playerFrameSprite_->GetPosition() + hpSpriteData_.at(2).position);

}

void PlayerUI::UpdatePlayerUI() {
	UpdateHP();
}

void PlayerUI::UpdateHP() {

	if (playerHP_->GetCurrentHP() == 0) {
		hpSprite_.at(0)->SetIsActive(false);
		hpSprite_.at(1)->SetIsActive(false);
		hpSprite_.at(2)->SetIsActive(false);
	}
	else if (playerHP_->GetCurrentHP() == 1) {
		hpSprite_.at(0)->SetIsActive(false);
		hpSprite_.at(1)->SetIsActive(false);
		hpSprite_.at(2)->SetIsActive(true);
	}
	else if (playerHP_->GetCurrentHP() == 2) {
		hpSprite_.at(0)->SetIsActive(false);
		hpSprite_.at(1)->SetIsActive(true);
		hpSprite_.at(2)->SetIsActive(true);
	}
	else if (playerHP_->GetCurrentHP() == 3) {
		hpSprite_.at(0)->SetIsActive(true);
		hpSprite_.at(1)->SetIsActive(true);
		hpSprite_.at(2)->SetIsActive(true);
	}
}

void PlayerUI::UpdateRevengeGage() {
	float revengeGage = playerRevengeGage_->GetCurrentRevengeBarGage();
	float barT = revengeGage / PlayerRevengeGage::kMaxRevengeBar;
#pragma region Bar
	revengeBarGaugeSprite_->SetPosition(
		{
		std::lerp((playerFrameSprite_->GetPosition().x + revengeBarGaugeSpriteData_.position.x) - revengeBarGaugeSpriteData_.scale.x * 0.5f,(playerFrameSprite_->GetPosition().x + revengeBarGaugeSpriteData_.position.x) ,barT),
		playerFrameSprite_->GetPosition().y + revengeBarGaugeSpriteData_.position.y
		}
	);
	revengeBarGaugeSprite_->SetScale(
		{
			std::lerp(0.0f, -revengeBarGaugeSpriteData_.scale.x,barT),
			revengeBarGaugeSpriteData_.scale.y
		}
	);
	revengeBarGaugeSprite_->SetTexcoordSize(
		{
			std::lerp(0.0f, -revengeBarGaugeSpriteData_.textureSize.x,barT),
			revengeBarGaugeSpriteData_.textureSize.y
		}
	);
	revengeBarGaugeSprite_->SetTexcoordBase(
		{
			std::lerp(0.0f, -revengeBarGaugeSpriteData_.textureSize.x,barT),
			revengeBarGaugeSpriteData_.textureSize.y
		}
	);
#pragma endregion
//
//	if (revengeGage >= PlayerRevengeGage::kMaxRevengeBar) {
//		//tutorial1_->SetIsActive(false);
//		//tutorial2_->SetIsActive(true);
//		//tutorial3_->SetIsActive(false);
//	}
//	else
//	if (Character::currentCharacterState_ == Character::kRunAway) {
//		//tutorial1_->SetIsActive(true);
//		//tutorial2_->SetIsActive(false);
//		//tutorial3_->SetIsActive(false);
//	}else
//	if (Character::currentCharacterState_ == Character::kChase) {
//		//tutorial1_->SetIsActive(false);
//		//tutorial2_->SetIsActive(false);
//		//tutorial3_->SetIsActive(true);
//	}
}

void PlayerUI::LoadJson() {
	JSON_OPEN("Resources/Data/Player/PlayerUI.json");

	JSON_OBJECT("userFrameSpriteData_");
	userFrameSpriteData_.Load();
	JSON_ROOT();

	JSON_OBJECT("userFrameSpriteData2_");
	userFrameSpriteData2_.Load();
	JSON_ROOT();

	JSON_OBJECT("playerFrameSpriteData_");
	playerFrameSpriteData_.Load();
	JSON_ROOT();

	JSON_OBJECT("toBossDistanceBarSpriteData_");
	toBossDistanceBarSpriteData_.Load();
	JSON_ROOT();

	JSON_OBJECT("toBossDistanceMeterSpriteData_");
	toBossDistanceMeterSpriteData_.Load();
	JSON_ROOT();

	JSON_OBJECT("toBossDistanceNumberSpriteData_");
	toBossDistanceNumberSpriteData_.Load();
	JSON_ROOT();

#pragma region HP

	for (uint32_t i = 0; i < hpSpriteData_.size(); i++) {
		JSON_OBJECT("hpSpriteData_" + std::to_string(i));
		hpSpriteData_.at(i).Load();
		JSON_ROOT();
	}

	JSON_ROOT();

#pragma endregion
#pragma region 復讐ゲージ
	// 棒
	JSON_OBJECT("revengeBarGageData_");
	revengeBarGaugeSpriteData_.Load();
	JSON_ROOT();
	// Base
	JSON_OBJECT("revengeBarGageBaseData_");
	revengeBarGaugeBaseSpriteData_.Load();
	JSON_ROOT();
	//アイコン
	JSON_OBJECT("revengeBarIconSpriteData_");
	revengeBarIconSpriteData_.Load();
	JSON_ROOT();

	JSON_OBJECT("toBossDistanceMeterSpriteData_");
	toBossDistanceMeterSpriteData_.Load();
	JSON_ROOT();

	JSON_OBJECT("toBossDistanceNumberSpriteData_");
	toBossDistanceNumberSpriteData_.Load();
	JSON_ROOT();

	// 円
	JSON_OBJECT("tutorial1Data_");
	tutorial1Data_.Load();
	JSON_ROOT();

	// 円
	JSON_OBJECT("tutorial2Data_");
	tutorial2Data_.Load();
	JSON_ROOT();

	// 円
	JSON_OBJECT("tutorial3Data_");
	tutorial3Data_.Load();
	JSON_ROOT();


	/*JSON_OBJECT("revengeCircleGageBaseData_");
	revengeCircleGageBaseData_.Load();
	JSON_ROOT();*/
#pragma endregion
	JSON_CLOSE();

}

void PlayerUI::SaveJson() {
	JSON_OPEN("Resources/Data/Player/PlayerUI.json");

	JSON_OBJECT("userFrameSpriteData_");
	userFrameSpriteData_.Save();
	JSON_ROOT();

	JSON_OBJECT("playerFrameSpriteData_");
	playerFrameSpriteData_.Save();
	JSON_ROOT();

	JSON_OBJECT("toBossDistanceBarSpriteData_");
	toBossDistanceBarSpriteData_.Save();
	JSON_ROOT();

	JSON_OBJECT("toBossDistanceMeterSpriteData_");
	toBossDistanceMeterSpriteData_.Save();
	JSON_ROOT();

	JSON_OBJECT("toBossDistanceNumberSpriteData_");
	toBossDistanceNumberSpriteData_.Save();
	JSON_ROOT();

#pragma region HP
	for (uint32_t i = 0; i < hpSpriteData_.size(); i++) {
		JSON_OBJECT("hpSpriteData_" + std::to_string(i));
		hpSpriteData_.at(i).Save();
		JSON_ROOT();
	}

	JSON_ROOT();

#pragma endregion
#pragma region 復讐ゲージ
	// 棒
	JSON_OBJECT("revengeBarGageData_");
	revengeBarGaugeSpriteData_.Save();
	JSON_ROOT();
	// 棒
	JSON_OBJECT("revengeBarGageBaseData_");
	revengeBarGaugeBaseSpriteData_.Save();
	JSON_ROOT();
	//アイコン
	JSON_OBJECT("revengeBarIconSpriteData_");
	revengeBarIconSpriteData_.Save();
	JSON_ROOT();

	// 円
	JSON_OBJECT("tutorial1Data_");
	tutorial1Data_.Save();
	JSON_ROOT();
	// 円
	JSON_OBJECT("tutorial2Data_");
	tutorial1Data_.Save();
	JSON_ROOT();
	// 円
	JSON_OBJECT("tutorial3Data_");
	tutorial1Data_.Save();
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
