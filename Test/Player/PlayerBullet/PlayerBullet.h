#pragma once

#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"

#include "Graphics/Model.h"

class PlayerBullet :
	public GameObject {
public:
	void Initialize();
	void Update();
	void Create(const Vector3& pos, const Vector3& velocity);
	void Reset();


	bool GetIsAlive() { return isAlive_; }
	void SetIsAlive(bool flag) { isAlive_ = flag; }
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);
	std::unique_ptr<ModelInstance> model_;

	std::unique_ptr<BoxCollider> collider_;

	Vector3 velocity_;

	bool isAlive_;
};