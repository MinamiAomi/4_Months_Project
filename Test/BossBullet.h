#pragma once

#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"

#include "Graphics/Model.h"

class BossBullet :
	public GameObject {
public:
	void Initialize(const Transform& parent, const Vector3& position, const Vector3& velocity);
	void Update();

	void SetVelocity(float velocity) { velocity_.z = velocity; }
	float GetVelocity() { return velocity_.z; }


	bool GetIsAlive() { return isAlive_; }
	void SetIsAlive(bool flag) { isAlive_ = flag; }
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);
	std::unique_ptr<ModelInstance> model_;

	std::unique_ptr<BoxCollider> collider_;

	Vector3 velocity_;

	bool isAlive_;

	uint32_t lifeTime_;
};