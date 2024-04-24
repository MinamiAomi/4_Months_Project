#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"

class UI {
private:
	struct SpriteData {
		Vector2 scale = { 64.0f,64.0f };
		float rotate = 0.0f;
		Vector2 position = {0.0f,0.0f};
		Vector4 color = { 1.0f,1.0f,1.0f,1.0 };
		Vector2 textureSize = { 0.0f,0.0f };
		Vector2 textureBase = { 0.0f,0.0f };
		int order = 0;

		void Load();
		void Save();
	};
public:
	void Initialize();
	void Update();
private:

	void LoadJson();
	void SaveJson();
	std::unique_ptr<Sprite> CreateSprite(const SpriteData& spriteData, std::string spriteName);
	void DrawImGui(SpriteData& spriteData, std::string string, Sprite* sprite);

	// 操作
	std::unique_ptr<Sprite> controll_;
	SpriteData controllData_;
};