#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Math/MathUtils.h"
#include "Graphics/Model.h"
#include "Player/Player.h"

class Floor :
	public GameObject {
public:
	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos);
	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { rotate_ = rotate; }
	void SetPosition(const Vector3& pos) { transform.translate = pos; }
	const Vector3& GetScale() { return transform.scale; }
	const Vector3& GetRotate() { return rotate_; }
	const Vector3& GetPosition() { return transform.translate; }
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	const Player* player_;

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;

	Vector3 rotate_;
};