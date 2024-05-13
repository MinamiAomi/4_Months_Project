#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"

class Pause {
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
	enum Select {
		Retry,
		ToTitle,

		SelectNum
	};
public:
	void Initialize();
	void Update();

	void SetIsPause(bool is) { isPause_ = is; }
	bool GetIsPause() { return isPause_; }

	bool GetOrderReset() { return orderReset_; }
	bool GetOrderToTitle() { return orderToTitle_; }
private:

	void PauseUpdate();
	void UnPauseUpdate();
	void LoadJson();
	void SaveJson();
	std::unique_ptr<Sprite> CreateSprite(const SpriteData& spriteData, std::string spriteName);
	void DrawImGui(SpriteData& spriteData, std::string string, Sprite* sprite);

	bool isPause_ = false;
	bool orderReset_ = false;
	bool orderToTitle_ = false;
	Select select_ = Retry;

	// 操作
	std::unique_ptr<Sprite> ToTitle_;
	SpriteData ToTitleData_;

	std::unique_ptr<Sprite> retry_;
	SpriteData retryData_;

	std::unique_ptr<Sprite> arrow_;
	SpriteData arrowData_;

	std::unique_ptr<Sprite> grey_;
	SpriteData greyData_;
};