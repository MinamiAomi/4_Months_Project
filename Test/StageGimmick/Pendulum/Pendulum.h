#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "Engine/Math/MathUtils.h"
#include "Player/Player.h"

//struct PendulumDesc {
//	Vector3 anchor;
//	float length;
//	float angle;
//	float gravity;
//	float angularVelocity;
//	float angularAcceleration;
//
//	void Update();
//	const Vector3 GetPosition();
//};


class Stick :
	public GameObject {
public:
	struct Desc {
		Vector3 scale;
	};

	void Initialize(const Transform* transform,const Desc& desc);
	void SetDesc(const Desc& desc);
	void Update();

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

class Ball :
	public GameObject {
public:
	struct Desc {
		Vector3 scale;
		float length;
		float gravity;
		float angle;
	};
	void Initialize(const Transform* transform, const Desc& desc);
	void Update();

	void SetIsActive(bool flag) {
		model_->SetIsActive(flag);
		collider_->SetIsActive(flag);
	}
	void SetDesc(const Desc& desc);
private:
	static const std::string kModelName;

	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;

	Vector3 rotate_;
	float length_;
	float angle_;
	float gravity_;
	float angularVelocity_;
	float angularAcceleration_;
};

class Pendulum :
	public GameObject {
public:
	struct Desc {
		Stick::Desc stickDesc;
		Ball::Desc ballDesc;
		Vector3 pos;
	};

	void Initialize(const Desc& desc);
	void Update();

	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetIsActive(bool flag) {
		stick_->SetIsActive(flag);
		ball_->SetIsActive(flag);
	}
	Desc& GetDesc() { return desc_; }
	void SetDesc(const Desc& desc) { 
		desc_= desc; 
		stick_->SetDesc(desc_.stickDesc);
		ball_->SetDesc(desc_.ballDesc);
	}
private:
	void UpdateTransform();

	const Boss* boss_;

	std::unique_ptr<Stick> stick_;
	std::unique_ptr<Ball> ball_;

	Vector3 rotate_;

	Desc desc_;
};