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
	static bool isEndFrame;

	virtual void Initialize(Player* player,Boss* boss,Camera* camera, const StageCamera* stageCamera);
	virtual void Update();
	virtual void Reset();
	void ZoomInOut(const Vector3& savePos, const Vector3& axis , const float t);
	static void SetFalseIsEndFrame() {
		isEndFrame = false;
	}


protected:
	Player* player_ = nullptr;
	Boss* boss_ = nullptr;
	Camera* camera_ = nullptr;
	const StageCamera* stageCamera_ = nullptr;
	Vector3 saveZoomPos_[3];
	bool isSetCamera_ = false;
	bool isInitialize_ = false;;

	int frame_ = 0;
};