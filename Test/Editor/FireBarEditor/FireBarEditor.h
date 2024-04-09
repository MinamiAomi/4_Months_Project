#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "StageGimmick/FireBar/FireBarManager.h"
#include "Collision/Collider.h"
#include "Graphics/Model.h"

class FireBarEditor
	: public GameObject {
public:
	void Initialize();
	void Update();

	void SetFireManager(FireBarManager* fireBarManager) { fireBarManager_ = fireBarManager; }
	void SaveFile(uint32_t stageName);
	void LoadFile(uint32_t stageName);
	void Clear();
private:
	static const std::string kModelName;

	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	FireBarManager* fireBarManager_;

	Vector3 centerRotate_;
	Vector3 barRotate_;
	float barRotateVelocity_;

	std::string fileName_;

	int stageIndex_;

	std::unique_ptr<ModelInstance> center_;
	std::unique_ptr<ModelInstance> bar_;
	Transform barTransform_;
	std::unique_ptr<BoxCollider> collider_;
	bool isCreate_;
};