#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "Math/Random.h"


class Camera;
class Player;
class Boss;
class Trap : 
	public GameObject {
public:
	struct Desc {
		Vector3 pos;
		float dropVelocity;
		float shotVelocity;
		float offset;
	};
	enum State {
		kDrop,
		kStay,
		kShot,

		kCount,
	};
	void Initialize(const Desc& desc);
	void Update();

	bool GetIsAlive() { return isAlive_; }
	const State GetState() const { return state_; }

	void SetPlayer(const Player* player) { player_ = player; }
	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	const Player* player_;
	const Boss* boss_;
	const Camera* camera_;

	std::unique_ptr<ModelInstance> model_;

	std::unique_ptr<BoxCollider> collider_;

	Desc desc_;

	State state_;
	
	bool isAlive_;

	Vector3 setPos_;
	Vector3 random_;

	Random::RandomNumberGenerator rnd_;

	float time_;
}; 