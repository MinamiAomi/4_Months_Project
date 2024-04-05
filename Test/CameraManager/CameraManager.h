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
	void Reset();
	void SetIsMove(bool flag);

	const StageCamera* GetStageCamera() { return stageCamera_.get(); }
	//const std::shared_ptr<Camera>& GetCamera() const;
private:
	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<StageCamera> stageCamera_;

	State state_;
};