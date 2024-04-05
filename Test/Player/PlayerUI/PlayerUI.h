#pragma once

#include <string>
#include <memory>

#include "Graphics/Sprite.h"

class PlayerUI {
private:
	struct SpriteData {
		Vector2 position;
		Vector2 scale;
		Vector2 textureSize;
		Vector2 textureBase;
		uint32_t order;
	};
public:
	void Initialize();
	void Update();
private:
	void DrawImGui(SpriteData& spriteData,std::string string, Sprite* sprite);

	std::unique_ptr<Sprite> hpSprit_;
	SpriteData hpSpriteData_;
};