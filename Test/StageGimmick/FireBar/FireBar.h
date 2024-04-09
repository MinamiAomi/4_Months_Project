#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "Engine/Math/MathUtils.h"
#include "Player/Player.h"

// バーとブロックで分けた

class Center :
	public GameObject {
public:
	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos);
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { rotate_ = rotate; }
	void SetPosition(const Vector3& pos) { transform.translate = pos; }
	const Vector3& GetScale() { return transform.scale; }
	const Vector3& GetRotate() { return rotate_; }
	const Vector3& GetPosition() { return transform.translate; }
	bool GetIsDown() { return isDown_; }
	bool GetOnceOnPlayer() { return onceOnPlayer_; }
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	static const std::string kModelName;

	const Player* player_;

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;
	Vector3 rotate_;


	bool onPlayer_;
	bool onceOnPlayer_;

	bool isDown_;
};

class Bar :
	public GameObject {
public:
	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos,float rotateVelocity);
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { rotate_ = rotate; }
	void SetPosition(const Vector3& pos) { pos_ = pos; }
	void SetRotateVelocity(float rotateVelocity) { rotateVelocity_= rotateVelocity; }
	const Vector3& GetScale() { return transform.scale; }
	const Vector3& GetRotate() { return rotate_; }
	const Vector3& GetPosition() { return pos_; }
	float GetRotateVelocity() { return rotateVelocity_; }
	void SetIsActive(bool flag);
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	static const std::string kModelName;

	const Player* player_;

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;

	Vector3 pos_;
	Vector3 rotate_;

	float rotateVelocity_;
};

class FireBar {
public:
	void Initialize(
		const Vector3& pos,
		const Vector3& centerScale, const Vector3& centerRotate,
		const Vector3& barScale, const Vector3& barRotate, float barRotateVelocity);

	void Update();

	void SetPlayer(const Player* player) { player_ = player; }

	Center* GetCenter() { return center_.get(); }
	Bar* GetBar() { return bar_.get(); }
	void SetPosition(const Vector3& pos) { pos_ = pos; }
	const Vector3& GetPosition() { return pos_; }
private:
	const Player* player_;

	std::unique_ptr<Center> center_;
	std::unique_ptr<Bar> bar_;

	Vector3 pos_;
};