#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Boss/Boss.h"
#include "Boss/BossStateManager/BossStateManager.h"
#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "Math/MathUtils.h"

class BossAttackTrigger :
	public GameObject {
public:
	struct Desc {
		BossStateManager::State state;
		float pos;
	};
	void Initialize(const Desc desc);
	void Update();

	void Reset();
	void SetBoss(const Boss* boss) { boss_ = boss; }
private:
	void OnCollision(const CollisionInfo& collisionInfo);
	const Boss* boss_;

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;

	bool isCollision_;
	Desc desc_;
};