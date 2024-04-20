#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Boss/BossStateManager/BossStateManager.h"
#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "Math/MathUtils.h"
#include "Math/Camera.h"
class Boss;
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
	Desc& GetDesc() { return desc_; }
	void SetDesc(const Desc& desc);
	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
	void SetIsAlive(bool flag);
	void SetIsModelAlive(bool flag) { return model_->SetIsActive(flag); }
	void SetIsColliderAlive(bool flag) { return collider_->SetIsActive(flag); }
private:
	void OnCollision(const CollisionInfo& collisionInfo);
	const Boss* boss_;
	const Camera* camera_;

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;

	bool isCollision_;
	Desc desc_;
};