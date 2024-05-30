#pragma once

#include <memory>

#include "DebugCamera.h"
#include "Math/Camera.h"

#include "StageCamera/StageCamera.h"

class Player;
class Boss;
class CameraManager {
public:
	enum State {
		kStageCamera,
		kDebugCamera,
		kMovieCamera,

		kCount,
	};
	CameraManager();
	void Initialize(const Player* player,const Boss* boss);
	void Update();
	void Reset();
	void SetIsMove(bool flag);

	const StageCamera* GetStageCamera() { return stageCamera_.get(); }
	Camera* GetMovieCamera() { return movieCamera_.get(); }
	void SetState(State state) { state_ = state; }
	const std::shared_ptr<Camera>& GetCamera() const;
	const std::shared_ptr<Camera>& GetDebugCamera() {
		return debugCamera_->GetCamera();
	}
	//const std::shared_ptr<Camera>& GetCamera() const;
private:
	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<StageCamera> stageCamera_;

	//movie
	std::shared_ptr<Camera> movieCamera_;

	State state_;
	bool isMove_;
	float distance_;
	bool isDebug_;
};