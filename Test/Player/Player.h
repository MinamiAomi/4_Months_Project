#pragma once
#include "Collision/Collider.h"
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Math/MathUtils.h"
#include "Graphics/Model.h"
#include "PlayerModel.h"

class FollowCamera;

class Player :
	public GameObject {
public:
	void Initialize();
	void Update();

	void Reset();

	const Vector3& GetLocalPos() const { return transform.translate; }
	const Matrix4x4& GetWorldMatrix() const { return transform.worldMatrix; }

private:
	void Move();
	void Jump();
	std::unique_ptr<ModelInstance> model_;
	//PlayerModel playerModel_;
	std::unique_ptr<BoxCollider> collider_;

	Vector3 acceleration_;
	Vector3 velocity_;
	bool onGround_;
#pragma region パラメーター
	float defaultSpeed_;
	float verticalSpeed_;
	float horizontalSpeed_;
	float jumpPower_;
	float gravity_;
#pragma endregion
#pragma region Json
	void DebugParam();
	void SaveJson();
	void LoadJson();
#pragma endregion
};