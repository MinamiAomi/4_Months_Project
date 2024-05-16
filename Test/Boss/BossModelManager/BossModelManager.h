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
		kFloorAll,
		kLongDistanceAttack,

		kBoss_2,

		kCount,
	};
	extern std::array<std::string, Parts::kCount> partsName_;
}

class BossModel :
	public GameObject {
public:
	void Initialize(uint32_t index);
	void Update();

	void SetRotate(const Vector3& rotate) { rotate_ = rotate; }
	const Vector3& GetRotate() const { return rotate_; }
	const Vector3& GetOffset()const { return offset_; }
	void SetColliderIsAlive(bool flag) { collider_->SetIsActive(flag); }
	void SetModelIsAlive(bool flag) { model_->SetIsActive(flag); }
	void SetIsAlive(bool flag) { 
		SetColliderIsAlive(flag);
		SetModelIsAlive(flag);
	}
	const std::unique_ptr<ModelInstance>& GetModel()const { return model_; }
	const std::unique_ptr<BoxCollider>& GetCollider()const { return collider_; }
private:
	virtual void OnCollision(const CollisionInfo& collisionInfo) = 0;
	void UpdateTransform();
	void DrawImGui();

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;
	Vector3 offset_;
	Vector3 rotate_;
	std::string name_;
};

class Boss_2:public BossModel {
private:
	void OnCollision(const CollisionInfo& collisionInfo) override;
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

class FloorAll :public BossModel {
private:
	void OnCollision(const CollisionInfo& collisionInfo) override;
};

class LongDistanceAttack :public BossModel {
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
