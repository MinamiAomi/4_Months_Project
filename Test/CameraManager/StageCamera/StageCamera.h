#pragma once
#include "Collision/GameObject.h"

#include <array>
#include <memory>

#include "CharacterState.h"
#include "Math/MathUtils.h"
#include "Math/Transform.h"
#include "Math/Camera.h"


class Player;
class StageCamera :
	public GameObject {
private:
	struct CameraParameter {
		Vector3 offset;
		Vector3 eulerAngle;
		Vector3 cameraVelocity;

		void Load();
		void Save();
	};
public:
	void Initialize();
	void Update();

	const std::shared_ptr<Camera>& GetCamera() const { return camera_; }
	void SetRenderManager();
	void Reset();

	void SetIsMove(bool flag) { isMove_ = flag; }
private:
	std::shared_ptr<Camera> camera_;
	std::array<CameraParameter, Character::State::kCount> cameraParam_;

	bool isMove_;
};