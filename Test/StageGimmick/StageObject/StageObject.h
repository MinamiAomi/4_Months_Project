#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>
#include <optional>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "Math/MathUtils.h"
#include "StageGimmick/StageGimmick.h"
#include "Player/Player.h"

class StageObject :
	public GameObject {
public:
	void Initialize(const StageGimmick::Desc& desc);
	void Update();

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	void UpdateTransform();
	//void OnCollision(const CollisionInfo& collisionInfo);

	const Player* player_;
	const Camera* camera_;

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;

	std::optional<StageGimmick::Collider> colliderDesc_;

	//Vector3 rotate_;

	StageGimmick::Desc desc_;
};