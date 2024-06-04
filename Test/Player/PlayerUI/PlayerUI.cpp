#include "PlayerUI.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Math/MathUtils.h"
#include"Engine/Graphics/GameWindow.h"
#include "Player/Player.h"
#include "Math/Camera.h"
#include "Movie.h"

void PlayerUI::Initialize() {
	LoadJson();

	//拡大比率統一
	playerFrameSpriteData_.scale = (Vector2(playerFrameSpriteData_.textureSize.x * userFrameSpriteRatioUI, playerFrameSpriteData_.textureSize.y * userFrameSpriteRatioUI));
	toBossDistanceBarSpriteData_.scale = (Vector2(toBossDistanceBarSpriteData_.textureSize.x * ratioUI, toBossDistanceBarSpriteData_.textureSize.y * ratioUI));
	toBossDistanceMeterSpriteData_.scale = (Vector2(toBossDistanceMeterSpriteData_.textureSize.x * ratioUI, toBossDistanceMeterSpriteData_.textureSize.y * ratioUI));
	toBossDistanceNumberSpriteData100_.scale = (Vector2(toBossDistanceNumberSpriteData100_.textureSize.x * ratioUI, toBossDistanceNumberSpriteData100_.textureSize.y * ratioUI));
	toBossDistanceNumberSpriteData10_.scale = (Vector2(toBossDistanceNumberSpriteData10_.textureSize.x * ratioUI, toBossDistanceNumberSpriteData10_.textureSize.y * ratioUI));
	toBossDistanceNumberSpriteData1_.scale = (Vector2(toBossDistanceNumberSpriteData1_.textureSize.x * ratioUI, toBossDistanceNumberSpriteData1_.textureSize.y * ratioUI));
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
	toBossDistanceNumberSprite100_ = CreateSprite(toBossDistanceNumberSpriteData100_, "ToBossDistanceNumber");
	toBossDistanceNumberSprite10_ = CreateSprite(toBossDistanceNumberSpriteData10_, "ToBossDistanceNumber");
	toBossDistanceNumberSprite1_ = CreateSprite(toBossDistanceNumberSpriteData1_, "ToBossDistanceNumber");
	//toBossDistanceBarSprite_->SetIsActive(false);
	//toBossDistanceMeterSprite_->SetIsActive(false);
	//toBossDistanceNumberSprite_->SetIsActive(false);

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
	tutorial3_->SetIsActive(true);

}

void PlayerUI::Update() {
	UpdatePlayerUI();
	// ジャンプしてない
	if (player_->GetCanFirstJump()) {
		userFrameSprite_->SetIsActive(true);
		userFrameSprite2_->SetIsActive(false);
	}
	// 一回ジャンプ
	else if(player_->GetCanSecondJump()) {
		userFrameSprite_->SetIsActive(false);
		userFrameSprite2_->SetIsActive(true);
	}
	// 二回ジャンプ
	else {
		userFrameSprite_->SetIsActive(true);
		userFrameSprite2_->SetIsActive(false);
	}
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Player")) {
		if (ImGui::TreeNode("PlayerUI")) {

			DrawImGui(userFrameSpriteData_, "userFrameSpriteData_", userFrameSprite_.get());

			DrawImGui(playerFrameSpriteData_, "playerFrameSpriteData_", playerFrameSprite_.get());

			DrawImGui(toBossDistanceBarSpriteData_, "toBossDistanceBarSpriteData_", toBossDistanceBarSprite_.get());

			DrawImGui(toBossDistanceMeterSpriteData_, "toBossDistanceMeterSpriteData_", toBossDistanceMeterSprite_.get());

			DrawImGui(toBossDistanceNumberSpriteData100_, "toBossDistanceNumberSpriteData100_", toBossDistanceNumberSprite100_.get());
			DrawImGui(toBossDistanceNumberSpriteData10_, "toBossDistanceNumberSpriteData10_", toBossDistanceNumberSprite10_.get());
			DrawImGui(toBossDistanceNumberSpriteData1_, "toBossDistanceNumberSpriteData1_", toBossDistanceNumberSprite1_.get());

			DrawImGui(revengeBarGaugeSpriteData_, "revengeBarGageSpriteData_", revengeBarGaugeSprite_.get());

			DrawImGui(revengeBarGaugeBaseSpriteData_, "revengeBarGageBaseSpriteData_", revengeBarGaugeBaseSprite_.get());

			DrawImGui(revengeBarIconSpriteData_, "revengeBarIconSpriteData_", revengeBarIconSprite_.get());

			DrawImGui(tutorial1Data_, "tutorial1Data_", tutorial1_.get());

			DrawImGui(tutorial2Data_, "tutorial2Data_", tutorial2_.get());

			DrawImGui(tutorial3Data_, "tutorial3Data_", tutorial3_.get());

			ImGui::Text("ボスとの距離 %f", player_->GetToBossDistance());

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

	UpdateToBossDistance();

	RECT rect;
	GetWindowRect(GameWindow::GetInstance()->GetHWND(), &rect);
	LONG width = rect.right - rect.left;
	LONG height = rect.bottom - rect.top;
	width = 1920;
	height = 1080;

	Matrix4x4 vpv = camera_->GetViewMatrix() * camera_->GetProjectionMatrix() ;
	vpv *= Matrix4x4::MakeScaling({ 1.0f, -1.0f, 1.0f }) * Matrix4x4::MakeTranslation({0.0f, 0.0f, 0.0f});
	vpv *= Matrix4x4::MakeViewport(0, 0, float(width), float(height), camera_->GetNearClip(), camera_->GetFarClip());
	Vector3 viewportPlayerPosition = vpv.ApplyTransformWDivide(player_->transform.worldMatrix.GetTranslate());
	Vector2 framePosition = Vector2(viewportPlayerPosition.x, viewportPlayerPosition.y);

	playerFrameSprite_->SetPosition(framePosition);

	//プレイヤーUIフレームを親にする
	UpdateRevengeGage(framePosition);
	//revengeBarGaugeSprite_->SetPosition(playerFrameSprite_->GetPosition() + revengeBarGaugeSpriteData_.position);
	revengeBarGaugeBaseSprite_->SetPosition(framePosition + revengeBarGaugeBaseSpriteData_.position);
	revengeBarIconSprite_->SetPosition(framePosition + revengeBarIconSpriteData_.position);
	toBossDistanceBarSprite_->SetPosition(framePosition + toBossDistanceBarSpriteData_.position);
	toBossDistanceMeterSprite_->SetPosition(framePosition + toBossDistanceMeterSpriteData_.position);
	toBossDistanceNumberSprite100_->SetPosition(framePosition + toBossDistanceNumberSpriteData100_.position);
	toBossDistanceNumberSprite10_->SetPosition(framePosition + toBossDistanceNumberSpriteData10_.position);
	toBossDistanceNumberSprite1_->SetPosition(framePosition + toBossDistanceNumberSpriteData1_.position);

	hpSprite_.at(0)->SetPosition(framePosition + hpSpriteData_.at(0).position);
	hpSprite_.at(1)->SetPosition(framePosition + hpSpriteData_.at(1).position);
	hpSprite_.at(2)->SetPosition(framePosition + hpSpriteData_.at(2).position);

	if (Movie::isPlaying) {
		playerFrameSprite_->SetIsActive(false);
		toBossDistanceBarSprite_->SetIsActive(false);
		toBossDistanceMeterSprite_->SetIsActive(false);
		toBossDistanceNumberSprite100_->SetIsActive(false);
		toBossDistanceNumberSprite10_->SetIsActive(false);
		toBossDistanceNumberSprite1_->SetIsActive(false);
		hpSprite_.at(0)->SetIsActive(false);
		hpSprite_.at(1)->SetIsActive(false);
		hpSprite_.at(2)->SetIsActive(false);
		revengeBarGaugeSprite_->SetIsActive(false);
		revengeBarGaugeBaseSprite_->SetIsActive(false);
		revengeBarIconSprite_->SetIsActive(false);
		tutorial1_->SetIsActive(false);
		tutorial2_->SetIsActive(false);
		tutorial3_->SetIsActive(false);
	}
	else {
		playerFrameSprite_->SetIsActive(true);
		toBossDistanceBarSprite_->SetIsActive(true);
		toBossDistanceMeterSprite_->SetIsActive(true);
		revengeBarGaugeSprite_->SetIsActive(true);
		revengeBarGaugeBaseSprite_->SetIsActive(true);
		revengeBarIconSprite_->SetIsActive(true);
	}
	
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
		hpSprite_.at(0)->SetIsActive(true);
		hpSprite_.at(1)->SetIsActive(false);
		hpSprite_.at(2)->SetIsActive(false);
	}
	else if (playerHP_->GetCurrentHP() == 2) {
		hpSprite_.at(0)->SetIsActive(true);
		hpSprite_.at(1)->SetIsActive(true);
		hpSprite_.at(2)->SetIsActive(false);
	}
	else if (playerHP_->GetCurrentHP() == 3) {
		hpSprite_.at(0)->SetIsActive(true);
		hpSprite_.at(1)->SetIsActive(true);
		hpSprite_.at(2)->SetIsActive(true);
	}
}

void PlayerUI::UpdateRevengeGage(const Vector2& framePosition) {
	float revengeGage = playerRevengeGage_->GetCurrentRevengeBarGage();
	float barT = revengeGage / PlayerRevengeGage::kMaxRevengeBar;
#pragma region Bar
	revengeBarGaugeSprite_->SetPosition(
		{
		std::lerp((framePosition.x + revengeBarGaugeSpriteData_.position.x) - revengeBarGaugeSpriteData_.scale.x * 0.5f,(framePosition.x + revengeBarGaugeSpriteData_.position.x) ,barT),
		framePosition.y + revengeBarGaugeSpriteData_.position.y
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

	if (Character::currentCharacterState_ == Character::kRunAway) {
		tutorial1_->SetIsActive(true);
		tutorial3_->SetIsActive(false);
	}else
	if (Character::currentCharacterState_ == Character::kChase) {
		tutorial1_->SetIsActive(false);
		tutorial3_->SetIsActive(true);
	}
}

void PlayerUI::UpdateToBossDistance() {
	if (player_->GetToBossDistance() >= 100) {
		int number = int(player_->GetToBossDistance());
		int eachNumber[3] = {};
		//百の位
		eachNumber[0] = number / 100;
		number = number % 100;
		//十の位
		eachNumber[1] = number / 10;
		number = number % 10;
		//一の位
		eachNumber[2] = number;

		toBossDistanceNumberSprite100_->SetIsActive(true);
		toBossDistanceNumberSprite10_->SetIsActive(true);
		toBossDistanceNumberSprite1_->SetIsActive(true);
		toBossDistanceNumberSpriteData100_.position = toBossDistanceNumberSpriteData100_.position;
		toBossDistanceNumberSpriteData10_.position = toBossDistanceNumberSpriteData100_.position + Vector2(15.0f, 0);
		toBossDistanceNumberSpriteData1_.position = toBossDistanceNumberSpriteData10_.position + Vector2(15.0f, 0);
		toBossDistanceMeterSpriteData_.position = toBossDistanceNumberSpriteData1_.position + Vector2(20.0f, -5.0f);

		toBossDistanceNumberSpriteData100_.textureBase.x = 75.0f * (eachNumber[0]);
		toBossDistanceNumberSpriteData10_.textureBase.x = 75.0f * (eachNumber[1]);
		toBossDistanceNumberSpriteData1_.textureBase.x = 75.0f * (eachNumber[2]);
		toBossDistanceNumberSprite100_->SetTexcoordBase(toBossDistanceNumberSpriteData100_.textureBase);
		toBossDistanceNumberSprite10_->SetTexcoordBase(toBossDistanceNumberSpriteData10_.textureBase);
		toBossDistanceNumberSprite1_->SetTexcoordBase(toBossDistanceNumberSpriteData1_.textureBase);

	}

	if (player_->GetToBossDistance() >= 10 && player_->GetToBossDistance() < 100) {
		int number = int(player_->GetToBossDistance());
		int eachNumber[2] = {};
		//十の位
		eachNumber[0] = number / 10;
		number = number % 10;
		//一の位
		eachNumber[1] = number;

		toBossDistanceNumberSprite100_->SetIsActive(false);
		toBossDistanceNumberSprite10_->SetIsActive(true);
		toBossDistanceNumberSprite1_->SetIsActive(true);
		toBossDistanceNumberSpriteData10_.position = toBossDistanceNumberSpriteData100_.position;
		toBossDistanceNumberSpriteData1_.position = toBossDistanceNumberSpriteData10_.position + Vector2(15.0f, 0);
		toBossDistanceMeterSpriteData_.position = toBossDistanceNumberSpriteData1_.position + Vector2(20.0f, -5.0f);

		toBossDistanceNumberSpriteData10_.textureBase.x = 75.0f * (eachNumber[0]);
		toBossDistanceNumberSpriteData1_.textureBase.x = 75.0f * (eachNumber[1]);
		toBossDistanceNumberSprite10_->SetTexcoordBase(toBossDistanceNumberSpriteData10_.textureBase);
		toBossDistanceNumberSprite1_->SetTexcoordBase(toBossDistanceNumberSpriteData1_.textureBase);

	}

	if (player_->GetToBossDistance() >= 0 && player_->GetToBossDistance() < 10) {
		int number = int(player_->GetToBossDistance());

		toBossDistanceNumberSprite100_->SetIsActive(false);
		toBossDistanceNumberSprite10_->SetIsActive(false);
		toBossDistanceNumberSprite1_->SetIsActive(true);
		toBossDistanceNumberSpriteData1_.position = toBossDistanceNumberSpriteData100_.position;
		toBossDistanceMeterSpriteData_.position = toBossDistanceNumberSpriteData1_.position + Vector2(20.0f, -5.0f);
		toBossDistanceNumberSpriteData1_.textureBase.x = 75.0f * (number);
		toBossDistanceNumberSprite1_->SetTexcoordBase(toBossDistanceNumberSpriteData1_.textureBase);

	}

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

	JSON_OBJECT("toBossDistanceNumberSpriteData100_");
	toBossDistanceNumberSpriteData100_.Load();
	JSON_ROOT();

	JSON_OBJECT("toBossDistanceNumberSpriteData10_");
	toBossDistanceNumberSpriteData10_.Load();
	JSON_ROOT();

	JSON_OBJECT("toBossDistanceNumberSpriteData1_");
	toBossDistanceNumberSpriteData1_.Load();
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

	//JSON_OBJECT("toBossDistanceNumberSpriteData_");
	//toBossDistanceNumberSpriteData_.Load();
	//JSON_ROOT();

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

	JSON_OBJECT("toBossDistanceNumberSpriteData100_");
	toBossDistanceNumberSpriteData100_.Save();
	JSON_ROOT();

	JSON_OBJECT("toBossDistanceNumberSpriteData10_");
	toBossDistanceNumberSpriteData10_.Save();
	JSON_ROOT();

	JSON_OBJECT("toBossDistanceNumberSpriteData1_");
	toBossDistanceNumberSpriteData1_.Save();
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
