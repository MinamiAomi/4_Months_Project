#pragma once
#include "Collision/GameObject.h"

#include <array>
#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Graphics/Model.h"

namespace BossParts {
	enum Parts {
		kBody,
		kRightArm,
		kLeftArm,

		kCount,
	};
	std::array<std::string, Parts::kCount> partsName_ = { // 修正
		"bossBody",
		"bossRightArm",
		"bossLeftArm",
	};
}

class BossModel :
	public GameObject {
public:
	void Initialize(uint32_t index);
	void Update();
private:
	virtual void OnCollision(const CollisionInfo& collisionInfo) = 0;
	void UpdateTransform();

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;
};

class Body :public BossModel {
private:
	void OnCollision(const CollisionInfo& collisionInfo) override;
};

class RightArm :public BossModel {
private:
	void OnCollision(const CollisionInfo& collisionInfo) override;
};

class LeftArm :public BossModel {
private:
	void OnCollision(const CollisionInfo& collisionInfo) override;
};

class BossModelManager {
public:

	void Initialize(const Transform* Transform);
	void Update();

	const std::unique_ptr<BossModel>& GetModel(BossParts::Parts parts) { return models_.at(parts); }
private:
	std::array<std::unique_ptr<BossModel>, BossParts::Parts::kCount> models_;
};
