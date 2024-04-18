#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "Engine/Math/MathUtils.h"
#include "Player/Player.h"
#include "Math/Camera.h"

class Stick :
	public GameObject {
public:
	
	void Initialize(const Transform* transform, float length, float scale);
	void Update();
	void SetDesc(float length, float scale);
	void SetIsActive(bool flag) {
		model_->SetIsActive(flag);
		collider_->SetIsActive(flag);
	}
private:
	static const std::string kModelName;

	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;
};

class Ball :
	public GameObject {
public:
	void Initialize(const Transform* transform, float length, float scale);
	void Update();
	void SetDesc(float length, float scale);
	void SetIsActive(bool flag) {
		model_->SetIsActive(flag);
		collider_->SetIsActive(flag);
	}
private:
	static const std::string kModelName;

	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;

	Vector3 rotate_;

};

class Pendulum :
	public GameObject {
public:
	struct Desc {
		Vector3 pos;
		Vector3 scale;

		float length;
		float gravity;
		float angle;
		float initializeAngle;
		float ballScale;
		float stickScale;
	};

	void Initialize(const Desc& desc);
	void Update();

	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetIsActive(bool flag) {
		stick_->SetIsActive(flag);
		ball_->SetIsActive(flag);
	}
	Desc& GetDesc() { return desc_; }
	void SetDesc(const Desc& desc);
	Ball* GetBall() { return ball_.get(); }

	void SetPlayer(const Player* player) { player_ = player; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	void UpdateTransform();

	const Player* player_;
	const Boss* boss_;
	const Camera* camera_;

	std::unique_ptr<Stick> stick_;
	std::unique_ptr<Ball> ball_;

	Desc desc_;

	float angle_;
	float angularAcceleration_;
	float angularVelocity_;
};