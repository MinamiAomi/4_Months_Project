#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"

#include "Player/PlayerHP/PlayerHP.h"
#include "Player/PlayerRevengGage/PlayerRevengeGage.h"

class Camera;
class Player;
class PlayerUI {
private:
	struct SpriteData {
		Vector2 scale = { 64.0f,64.0f };
		float rotate;
		Vector2 position;
		Vector4 color = { 1.0f,1.0f,1.0f,1.0 };
		Vector2 textureSize;
		Vector2 textureBase;
		int order;

		void Load();
		void Save();
	};
	float ratioUI = 0.4f;
	float userFrameSpriteRatioUI = 0.25f;

public:
	void Initialize();
	void Update();

	void SetPlayer(Player* player) { player_ = player; }
	void SetPlayerHP(PlayerHP* playerHP) { playerHP_ = playerHP; }
	void SetPlaterRevengeGage(PlayerRevengeGage* playerRevengeGage) { playerRevengeGage_ = playerRevengeGage; }
	void SetCamera(Camera* camera) { camera_ = camera; }
private:
	void UpdateHP();
	void UpdateRevengeGage();
	void UpdatePlayerUI();

	void LoadJson();
	void SaveJson();
	std::unique_ptr<Sprite> CreateSprite(const SpriteData& spriteData, std::string spriteName);
	void DrawImGui(SpriteData& spriteData, std::string string, Sprite* sprite);

	Player* player_;
	PlayerHP* playerHP_;
	PlayerRevengeGage* playerRevengeGage_;
	Camera* camera_;

#pragma region 全体フレーム

	std::unique_ptr<Sprite> userFrameSprite_;
	SpriteData userFrameSpriteData_;

#pragma endregion

#pragma region プレイヤーUIフレーム

	std::unique_ptr<Sprite> playerFrameSprite_;
	SpriteData playerFrameSpriteData_;

#pragma endregion

#pragma region ボスとの距離

	std::unique_ptr<Sprite> toBossDistanceBarSprite_;
	SpriteData toBossDistanceBarSpriteData_;

	std::unique_ptr<Sprite> toBossDistanceMeterSprite_;
	SpriteData toBossDistanceMeterSpriteData_;

	std::unique_ptr<Sprite> toBossDistanceNumberSprite_;
	SpriteData toBossDistanceNumberSpriteData_;

#pragma endregion

#pragma region HP
	// HPUI
	std::array< std::unique_ptr<Sprite>, PlayerHP::kMaxHP> hpSprite_;
	std::array<SpriteData, PlayerHP::kMaxHP> hpSpriteData_;
	Vector2 hpCenter_;
	Vector2 hpSpriteOffset_;

#pragma endregion
#pragma region 復讐ゲージ
	// 棒のとこ
	std::unique_ptr<Sprite> revengeBarGaugeSprite_;
	SpriteData revengeBarGaugeSpriteData_;
	// Base
	std::unique_ptr<Sprite> revengeBarGaugeBaseSprite_;
	SpriteData revengeBarGaugeBaseSpriteData_;
	//アイコン
	std::unique_ptr<Sprite> revengeBarIconSprite_;
	SpriteData revengeBarIconSpriteData_;

	// tutorial
	std::unique_ptr<Sprite> tutorial1_;
	SpriteData tutorial1Data_;

	// tutorial
	std::unique_ptr<Sprite> tutorial2_;
	SpriteData tutorial2Data_;

	// tutorial
	std::unique_ptr<Sprite> tutorial3_;
	SpriteData tutorial3Data_;

	//// Base
	//std::unique_ptr<Sprite> revengeCircleGageBase_;
	//SpriteData revengeCircleGageBaseData_;
#pragma endregion

};