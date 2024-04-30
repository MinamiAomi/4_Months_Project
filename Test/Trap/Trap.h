#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Collision/Collider.h"
#include "Graphics/Model.h"

class Camera;
class Player;
class Trap : 
	public GameObject {
public:
	struct Desc {
		Vector3 pos;
		float velocity;
		float offset;
	};
	void Initialize(const Desc& desc);
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	const Player* player_;
	const Camera* camera_;

	std::unique_ptr<ModelInstance> model_;

	std::unique_ptr<BoxCollider> collider_;

	Desc desc_;

	bool isMove_;

	bool isAlive_;
}; 