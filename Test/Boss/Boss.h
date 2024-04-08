#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"

#include "Graphics/Model.h"
class Boss :
	public GameObject {
public:
	void Initialize();
	void Update();

	void Reset();
	void SetIsMove(bool flag) { isMove_ = flag; }
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);
	std::unique_ptr<ModelInstance> model_;

	std::unique_ptr<BoxCollider> collider_;

	Vector3 velocity_;
	Vector3 offset_;

	bool isMove_;
};