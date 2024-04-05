#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"

#include "CameraManager/StageCamera/StageCamera.h"
#include "Graphics/Model.h"
#include "Math/MathUtils.h"
#include "PlayerModel.h"

class Player :
	public GameObject {
public:
	void Initialize();
	void Update();

	void Reset();

	const Vector3& GetLocalPos() const { return transform.translate; }
	const Matrix4x4& GetWorldMatrix() const { return transform.worldMatrix; }

	void SetStageCamera(const StageCamera* stageCamera) { stageCamera_ = stageCamera; }
private:
	void Move();
	void Jump();

	const StageCamera* stageCamera_;

	std::unique_ptr<ModelInstance> model_;
	//PlayerModel playerModel_;
	std::unique_ptr<BoxCollider> collider_;

	Vector3 acceleration_;
	Vector3 velocity_;
	bool onGround_;
	bool canSecondJump_;

#pragma region パラメーター
	float defaultSpeed_;
	float verticalSpeed_;
	float horizontalSpeed_;
	float jumpPower_;
	float gravity_;
	float limitLine_;
#pragma endregion
#pragma region Json
	void DebugParam();
	void SaveJson();
	void LoadJson();
#pragma endregion
};