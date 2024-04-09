#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "Engine/Math/MathUtils.h"
#include "Player/Player.h"

class Pendulum :
	public GameObject {
public:
	void Initialize(
		const Vector3& scale, const Vector3& rotate, 
		const Vector3& ballScale, const Vector3& ballRotate,
		const Vector3& pos,float length, float speed,float angle);
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetStickScale(const Vector3& scale) { transform.scale = scale; }
	void SetStickRotate(const Vector3& rotate) { rotate_ = rotate; }
	void SetPosition(const Vector3& pos) { pos_ = pos; }
	const Vector3& GetStickScale() { return transform.scale; }
	const Vector3& GetStickRotate() { return rotate_; }
	const Vector3& GetPosition() { return pos_; }

	void SetBallScale(const Vector3& scale) { ballTransform_.scale = scale; }
	void SetBallRotate(const Vector3& rotate) { ballRotate_ = rotate; }
	const Vector3& GetBallScale() { return ballTransform_.scale; }
	const Vector3& GetBallRotate() { return ballRotate_; }

	void SetSpeed(float speed) { speed_ = speed; }
	float GetSpeed() { return speed_; }
	void SetAngle(float angle) { angle_ = angle; }
	float GetAngle() { return angle_; }
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	static const std::string kStickName;
	static const std::string kBallName;

	const Player* player_;

	std::unique_ptr<ModelInstance> stick_;
	std::unique_ptr<ModelInstance> ball_;
	Transform ballTransform_;

	std::unique_ptr<BoxCollider> collider_;

	Vector3 rotate_;
	Vector3 ballRotate_;
	Vector3 pos_;

	float speed_;
	float length_;
	float angle_;
};