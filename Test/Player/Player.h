#pragma once
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

	void SetFollowCamera(const std::shared_ptr<FollowCamera>& followCamera) { followCamera_ = followCamera; }

	const Vector3& GetLocalPos() { return transform.translate; }
	const Matrix4x4& GetWorldMatrix() { return transform.worldMatrix; }
private:
	void Move();
	void Jump();
	std::unique_ptr<ModelInstance> model_;
	//PlayerModel playerModel_;
	std::weak_ptr<FollowCamera> followCamera_;

#pragma region パラメーター
	float defaultSpeed_;
	float verticalSpeed_;
	float horizontalSpeed_;
#pragma endregion
#pragma region Json
	void DebugParam();
	void SaveJson();
	void SaveParameter(float param, std::string name);
	void LoadParameter(float& param, std::string name);
	void SaveParameter(const Vector2& param, std::string name);
	void LoadParameter(Vector2& param, std::string name);
	void SaveParameter(const Vector3& param, std::string name);
	void LoadParameter(Vector3& param, std::string name);
	void SaveParameter(const Vector4& param, std::string name);
	void LoadParameter(Vector4& param, std::string name);
	std::string fileName_;
#pragma endregion
};