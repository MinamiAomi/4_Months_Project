#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"

#include "Player/PlayerHP/PlayerHP.h"
#include "Player/PlayerRevengGage/PlayerRevengeGage.h"

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
public:
	void Initialize();
	void Update();

	void SetPlayerHP(PlayerHP* playerHP) { playerHP_ = playerHP; }
	void SetPlaterRevengeGage(PlayerRevengeGage* playerRevengeGage) { playerRevengeGage_ = playerRevengeGage; }
private:
	void UpdateHP();
	void UpdateRevengeGage();

	void LoadJson();
	void SaveJson();
	std::unique_ptr<Sprite> CreateSprite(const SpriteData& spriteData, std::string spriteName);
	void DrawImGui(SpriteData& spriteData, std::string string, Sprite* sprite);

	PlayerHP* playerHP_;
	PlayerRevengeGage* playerRevengeGage_;
#pragma region HP
	// HPUI
	std::array< std::unique_ptr<Sprite>, PlayerHP::kMaxHP> hpSprit_;
	std::array<SpriteData, PlayerHP::kMaxHP> hpSpriteData_;
	Vector2 hpCenter_;
	// HPのBase
	std::unique_ptr<Sprite> hpBaseSprit_;
	SpriteData hpBaseSpriteData_;
#pragma endregion
#pragma region 復讐ゲージ
	// 棒のとこ
	std::unique_ptr<Sprite> revengeBarGage_;
	SpriteData revengeBarGageData_;
	// Base
	std::unique_ptr<Sprite> revengeBarGageBase_;
	SpriteData revengeBarGageBaseData_;

	// 円のとこ
	std::unique_ptr<Sprite> revengeCircleGage_;
	SpriteData revengeCircleGageData_;

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