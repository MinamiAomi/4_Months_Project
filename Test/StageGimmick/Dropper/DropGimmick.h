#pragma once
#include "Collision/GameObject.h"

#include <list>
#include <vector>
#include <memory>

#include "Audio/AudioSource.h"
#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "StageGimmick/StageGimmick.h"
#include "Math/Random.h"
#include "Engine/Audio/AudioSource.h"

class Player;
class Camera;
class Boss;

class DropperBall :
	public GameObject, public StageGimmick::StageGimmickNumber {
public:
	struct Desc {
		StageGimmick::Desc desc;
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

	void SetPlayer(const Player* player) { player_ = player; }
	void SetBoss(Boss* boss) { boss_ = boss; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Player* player_;
	Boss* boss_;
	const Camera* camera_;

	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	std::unique_ptr<BoxCollider> collider_;
	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<AudioSource> kickSE_;
	std::unique_ptr<AudioSource> hitSE_;
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
	std::unique_ptr<ModelInstance> switchBase_;
	std::unique_ptr<ModelInstance> switch_;
	std::unique_ptr<AudioSource> switchSE_;
	Transform switchTransform_;

	Desc desc_;
	
	bool isPushed_;

	// 0~1
	float time_;
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

class DropperBallManager {
public:
	void Create(const DropperBall::Desc& desc, uint32_t index);
	void Update();
	
	void Reset();
	
	void Delete(DropperBall* ball);

	void SetPlayer(const Player* player) { player_ = player; }
	void SetBoss(Boss* boss) { boss_ = boss; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
	std::list<std::unique_ptr<DropperBall>>& GetDropGimmicks() { return dropperBalls_; }
private:
	const Player* player_;
	Boss* boss_;
	const Camera* camera_;

	std::list<std::unique_ptr<DropperBall>>dropperBalls_;
};

class DropGimmick :
	public StageGimmick::StageGimmickNumber {
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
	void SetDropperBallManager(DropperBallManager* dropperBallManager) { dropperBallManager_ = dropperBallManager; }
private:
	const Player* player_;
	const Boss* boss_;
	const Camera* camera_;
	DropperBallManager* dropperBallManager_;

	std::vector<std::unique_ptr<Switch>>switch_;
	std::vector<std::unique_ptr<Dropper>>dropper_;
	bool isCreate_;
};