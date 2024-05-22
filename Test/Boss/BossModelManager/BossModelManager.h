#pragma once
#include "Collision/GameObject.h"

#include <map>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "Graphics/Skeleton.h"

namespace BossParts {
	enum Parts {
		kBossBody,
		kFloorAll,
		kLongDistanceAttack,
		kCount,
	};
	extern std::array<std::string, Parts::kCount> partsName_;
}

class BossModel :
	public GameObject {
public:
	struct Parts {
		std::shared_ptr<Animation> animation;
		std::shared_ptr<Skeleton> skeleton;
		std::map<std::string, std::unique_ptr<BoxCollider>> parts;

		Parts() = default;
		Parts(const Parts&) = delete; // コピーコンストラクタを削除
		Parts& operator=(const Parts&) = delete; // コピー代入演算子を削除
		Parts(Parts&&) = default; // デフォルトの移動コンストラクタ
		Parts& operator=(Parts&&) = default; // デフォルトの移動代入演算子

		void SetIsCollision(bool flag);
		void UpdateCollider(const Matrix4x4& worldMat);
		void InitializeCollider(std::vector<std::string> nameList, std::string colliderName);
	};

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
	void SetColliderIsCollision(bool flag) {
		SetColliderIsAlive(flag);
	}
	const std::unique_ptr<ModelInstance>& GetModel()const { return model_; }
	const std::unique_ptr<BoxCollider>& GetCollider()const { return collider_; }

	void AddAnimation( std::vector<std::string> nameList, std::string colliderName);

	Parts& GetAnimation(uint32_t index) { return parts_.at(index); }
private:
	virtual void OnCollision(const CollisionInfo& collisionInfo) = 0;
	void UpdateTransform();
	void DrawImGui();

	std::vector<Parts> parts_;
	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;
	Vector3 offset_;
	Vector3 rotate_;
	std::string name_;
};

class BossBody :public BossModel {
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
