#pragma once
#include "Collision/GameObject.h"

#include <vector>
#include <memory>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "StageGimmick/StageGimmick.h"

class Player;
class Camera;
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
	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Player* player_;
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
	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Player* player_;
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
	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Player* player_;
	const Camera* camera_;

	std::vector<std::unique_ptr<Switch>>switch_;
	std::vector<std::unique_ptr<Dropper>>dropper_;
};