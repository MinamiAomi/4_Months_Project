#pragma once
#include "Collision/GameObject.h"

#include <list>
#include <vector>
#include <memory>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "StageGimmick/StageGimmick.h"
#include "Math/Random.h"

class Player;
class Camera;
class Boss;

class DropperBall :
	public GameObject {
public:
	enum State {
		kDrop,
		kStay,
		kShot,

		kCount,
	};

	void Initialize(const Vector3& pos);
	void Update();

	bool GetIsAlive() { return isAlive_; }

	void SetPlayer(const Player* player) { player_ = player; }
	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Player* player_;
	const Boss* boss_;
	const Camera* camera_;

	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	std::unique_ptr<BoxCollider> collider_;
	std::unique_ptr<ModelInstance> model_;
	State state_;
	bool isAlive_;

	float time_;
	Vector3 setPos_;
	Vector3 random_;
	Random::RandomNumberGenerator rnd_;
};

class Switch :
	public GameObject {
public:
	struct Desc {
		StageGimmick::Desc desc;
		uint32_t index;
	};
	void Initialize(const Desc& desc);
	void Update();

	bool GetIsPushed() { return isPushed_; }

	void SetPlayer(const Player* player) { player_ = player; }
	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Player* player_;
	const Boss* boss_;
	const Camera* camera_;

	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);
	

	std::unique_ptr<BoxCollider> collider_;
	std::unique_ptr<ModelInstance> model_;
	Desc desc_;
	
	bool isPushed_;
};

class Dropper :
	public GameObject{
public:
	struct Desc {
		StageGimmick::Desc desc;
		uint32_t index;
	};
	void Initialize(const Desc& desc);
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Player* player_;
	const Boss* boss_;
	const Camera* camera_;

	void OnCollision(const CollisionInfo& collisionInfo);
	void UpdateTransform();
	

	std::unique_ptr<BoxCollider> collider_;
	std::unique_ptr<ModelInstance> model_;
	Desc desc_;
};

class DropGimmick {
public:
	struct Desc {
		std::vector<Switch::Desc> switchDesc;
		std::vector<Dropper::Desc> dropperDesc;
	};
	void Initialize(const Desc& desc);
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Player* player_;
	const Boss* boss_;
	const Camera* camera_;

	std::vector<std::unique_ptr<Switch>>switch_;
	std::vector<std::unique_ptr<Dropper>>dropper_;
	std::list<std::unique_ptr<DropperBall>> dropperBall_;
	bool isCreate_;
};