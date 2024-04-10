#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "StageGimmick/Pendulum/PendulumManager.h"
#include "Collision/Collider.h"
#include "Graphics/Model.h"

class PendulumEditor
	: public GameObject {
private:
	struct PendulumDesc {
		Vector3 anchor;
		float length;
		float angle;
		float gravity;
		float angularVelocity;
		float angularAcceleration;
		
		void Update();
		const Vector3 GetPosition();
	};
public:
	void Initialize();
	void Update();

	void SetPendulumManager(PendulumManager* pendulumManager) { pendulumManager_ = pendulumManager; }
	void SaveFile(uint32_t stageName);
	void LoadFile(uint32_t stageName);
	void Clear();
private:
	static const std::string kModelName;

	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	PendulumManager* pendulumManager_;

	PendulumDesc pendulum_;
	float speed_;
	float angle_;
	float length_;
	Vector3 pos_;
	Vector3 rotate_;
	Vector3 ballRotate_;

	std::string fileName_;

	int stageIndex_;

	std::unique_ptr<ModelInstance> stick_;
	std::unique_ptr<ModelInstance> ball_;
	Transform ballTransform_;
	std::unique_ptr<BoxCollider> collider_;
	bool isCreate_;
};