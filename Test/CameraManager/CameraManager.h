#pragma once

#include <memory>

#include "DebugCamera.h"
#include "Math/Camera.h"
#include "Player/Player.h"
#include "StageCamera/StageCamera.h"

class CameraManager {
public:
	enum State {
		kStageCamera,
		kDebugCamera,

		kCount,
	};

	void Initialize(Player* player);
	void Update();

	const std::shared_ptr<Camera>& GetCamera() const;
private:
	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<StageCamera> stageCamera_;

	State state_;
};