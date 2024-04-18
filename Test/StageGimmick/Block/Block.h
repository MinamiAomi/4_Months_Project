#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "Engine/Math/MathUtils.h"
#include "Player/Player.h"

#include "StageGimmick/StageGimmick.h"
#include "Math/Camera.h"
class Block :
	public GameObject {
public:
	void Initialize(const StageGimmick::Desc& desc);
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { rotate_ = rotate; }
	void SetPosition(const Vector3& pos) { transform.translate = pos; }
	const Vector3& GetScale() { return transform.scale; }
	const Vector3& GetRotate() { return rotate_; }
	const Vector3& GetPosition() { return transform.translate; }

	void GetCamera(const Camera* camera) { camera_ = camera; }
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	const Player* player_;
	const Camera* camera_;

	std::unique_ptr<ModelInstance> model_;

	std::unique_ptr<BoxCollider> collider_;

	StageGimmick::Collider colliderDesc_;

	Vector3 rotate_;

	StageGimmick::Desc desc_;

	bool onPlayer_;
	bool onceOnPlayer_;
};