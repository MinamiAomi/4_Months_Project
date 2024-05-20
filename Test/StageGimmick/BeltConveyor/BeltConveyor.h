#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>
#include <optional>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "Engine/Math/MathUtils.h"
#include "Player/Player.h"

#include "StageGimmick/StageGimmick.h"
class BeltConveyor :
	public GameObject{
public:
	struct Desc {
		StageGimmick::Desc desc;
		float velocity;
	};
	void Initialize(const Desc& desc);
	void Update();

	void SetPlayer(Player* player) { player_ = player; }

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	Player* player_;
	const Camera* camera_;

	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	std::unique_ptr<ModelInstance> model_;

	std::unique_ptr<BoxCollider> collider_;

	Desc desc_;
};