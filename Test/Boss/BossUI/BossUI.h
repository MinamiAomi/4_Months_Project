#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"

#include "Boss/BossHP/BossHP.h"

class BossUI {
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

	void SetBossHP(const BossHP* bossHP) {	bossHP_ = bossHP; }
private:
	const BossHP* bossHP_;

	// HP
	std::unique_ptr<Sprite> hp_;
	SpriteData hpData_;
};