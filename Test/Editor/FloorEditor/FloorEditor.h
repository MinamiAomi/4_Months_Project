#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "StageGimmick/Floor/FloorManager.h"
#include "Collision/Collider.h"
#include "Graphics/Model.h"

class FloorEditor
	: public GameObject {
public:
	void Initialize();
	void Update();

	void SetFloorManager(FloorManager* floorManager) { floorManager_ = floorManager; }
	void SaveFile(uint32_t stageName);
	void Clear();
private:
	static const std::string kModelName;

	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	FloorManager* floorManager_;

	Vector3 rotate_;

	std::string fileName_;

	int stageIndex_;

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;
	bool isCreate_;
};