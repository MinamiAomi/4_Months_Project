#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"

#include "Player/PlayerHP/PlayerHP.h"

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
	};
public:
	void Initialize();
	void Update();

	void SetPlayerHP(PlayerHP* playerHP) { playerHP_ = playerHP; }
private:
	void DrawImGui(SpriteData& spriteData, std::string string, Sprite* sprite);

	PlayerHP* playerHP_;
#pragma region HP
	// HPUI
	std::array< std::unique_ptr<Sprite>, PlayerHP::kMaxHP> hpSprit_;
	std::array<SpriteData, PlayerHP::kMaxHP> hpSpriteData_;
	// HPの下円
	std::unique_ptr<Sprite> hpBaseSprit_;
	SpriteData hpBaseSpriteData_;
#pragma endregion
};