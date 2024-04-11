#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"

#include "Graphics/Model.h"
#include "BossModelManager/BossModelManager.h"

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
	
	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<BossModelManager> bossModelManager_;

	Vector3 velocity_;
	Vector3 offset_;

	bool isMove_;
};