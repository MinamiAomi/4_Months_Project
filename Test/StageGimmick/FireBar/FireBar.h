#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "Engine/Math/MathUtils.h"
#include "Player/Player.h"
#include "Math/Camera.h"
#include "StageGimmick/StageGimmick.h"

// バーとブロックで分けた
class BarChildren :
	public GameObject {
public:
	void Initialize(uint32_t count);
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetIsActive(bool flag);
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	static const std::string kModelName;

	const Player* player_;

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;
};


class Bar :
	public GameObject {
public:
	struct Desc {
		float barInitialAngle;
		float rotateVelocity;
		uint32_t length;
	};
	void Initialize(const Desc& desc);
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetRotateVelocity(float rotateVelocity) { rotateVelocity_ = rotateVelocity; }
	float GetRotateVelocity() { return rotateVelocity_; }
	void SetDesc(const Desc& desc);
	void SetIsActive(bool flag);
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);
	const Player* player_;

	float angle_;

	float rotateVelocity_;

	std::vector<std::unique_ptr<BarChildren>> barChildren_;
};

class FireBar :
	public GameObject {
public:
	struct Desc {
		StageGimmick::Desc desc;
		Bar::Desc barDesc;
	};

	void Initialize(const Desc& desc);

	void Update();

	void SetPlayer(const Player* player) { player_ = player; }

	Bar* GetBar() { return bar_.get(); }

	Desc& GetDesc() { return desc_; }
	void SetDesc(const Desc& desc);
	void SetIsActive(bool flag);

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	void OnCollision(const CollisionInfo& collisionInfo);
	void UpdateTransform();

	const Camera* camera_;
	const Player* player_;

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;

	std::optional<StageGimmick::Collider> colliderDesc_;

	//Vector3 rotate_;

	bool onPlayer_;
	bool onceOnPlayer_;
	bool isDown_;

	Desc desc_;

	std::unique_ptr<Bar> bar_;
};