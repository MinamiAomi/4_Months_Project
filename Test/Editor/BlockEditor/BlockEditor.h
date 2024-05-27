#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "StageGimmick/Block/BlockManager.h"
#include "Collision/Collider.h"
#include "Graphics/Model.h"

class BlockEditor 
	: public GameObject {
public:
	void Initialize();
	void Update();

	void SetBlockManager(BlockManager* blockManager) { blockManager_ = blockManager; }
	void SaveFile(uint32_t stageName);
	void Clear();
private:
	static const std::string kModelName;

	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	BlockManager* blockManager_;

	Vector3 rotate_;

	std::string fileName_;

	int stageIndex_;

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<BoxCollider> collider_;
	bool isCreate_;
};