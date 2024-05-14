#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"
#include "Boss/Boss.h"
#include "Player/Player.h"
#include "Math/Camera.h"
#include "CameraManager/StageCamera/StageCamera.h"
class Movie {
public:

	static bool isPlaying;

	virtual void Initialize(Player* player,Boss* boss,Camera* camera, const StageCamera* stageCamera);
	virtual void Update();
	virtual void Reset();

protected:
	Player* player_ = nullptr;
	Boss* boss_ = nullptr;
	Camera* camera_ = nullptr;
	const StageCamera* stageCamera_ = nullptr;
	bool isSetCamera_ = false;

	int test = 120;
};