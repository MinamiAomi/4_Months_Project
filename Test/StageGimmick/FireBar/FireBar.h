#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "Engine/Math/MathUtils.h"
#include "Player/Player.h"
#include "Math/Camera.h"

// バーとブロックで分けた
class Bar :
	public GameObject {
public:
	struct Desc {
		Vector3 barInitialAngle;
		float rotateVelocity;
		float length;
	};
	void Initialize(const Desc& desc);
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetRotate(const Vector3& rotate) { rotate_ = rotate; }
	void SetRotateVelocity(float rotateVelocity) { rotateVelocity_ = rotateVelocity; }
	const Vector3& GetRotate() { return rotate_; }
	float GetRotateVelocity() { return rotateVelocity_; }
	void SetDesc(const Desc& desc);
	void SetIsActive(bool flag);
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	static const std::string kModelName;

	const Player* player_;

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;

	Vector3 rotate_;

	float rotateVelocity_;
};

class FireBar :
	public GameObject {
public:
	struct Desc {
		Transform transform;
		Bar::Desc barDesc;
	};

	void Initialize(const Desc& desc);

	void Update();

	void SetPlayer(const Player* player) { player_ = player; }

	Bar* GetBar() { return bar_.get(); }

	Desc& GetDesc() { return desc_; }
	void SetDesc(const Desc& desc);
	void SetIsActive(bool flag);

	const Vector3& GetRotate() const { return rotate_; }
	void SetRotate(const Vector3& rotate) { rotate_ = rotate; }

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	void OnCollision(const CollisionInfo& collisionInfo);
	void UpdateTransform();

	const Camera* camera_;
	const Player* player_;

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;

	Vector3 rotate_;

	bool onPlayer_;
	bool onceOnPlayer_;
	bool isDown_;

	Desc desc_;

	std::unique_ptr<Bar> bar_;
};