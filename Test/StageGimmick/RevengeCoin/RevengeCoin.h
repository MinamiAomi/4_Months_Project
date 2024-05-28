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
#include "Math/Camera.h"
class RevengeCoin :
	public GameObject, public StageGimmick::StageGimmickNumber {
public:
	struct Desc {
		StageGimmick::Desc desc;
	};
	void Initialize(const Desc& desc);
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }

	void GetCamera(const Camera* camera) { camera_ = camera; }
	void SetIsAlive(bool flag) { isAlive_ = flag; }
	bool SetIsAlive() { return isAlive_; }
	void Reset();
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	const Player* player_;
	const Camera* camera_;

	std::unique_ptr<ModelInstance> model_;

	std::unique_ptr<BoxCollider> collider_;

	std::optional<StageGimmick::Collider> colliderDesc_;

	bool isAlive_;

};