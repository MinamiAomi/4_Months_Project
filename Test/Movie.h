#pragma once

#include <array>
#include <string>
#include <memory>
#include <functional>

#include "Graphics/Sprite.h"
#include "Boss/Boss.h"
#include "Player/Player.h"
#include "Math/Camera.h"
#include "CameraManager/StageCamera/StageCamera.h"
#include "Scene/SceneManager.h"
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
	bool isInitialize_ = false;;

	int frame_ = 0;
};