#pragma once
#include <memory>
#include <string>

#include "Block/BlockManager.h"
#include "Collision/GameObject.h"
#include "Graphics/Model.h"

class BlockEditor 
	: public GameObject {
public:
	void Initialize();
	void Update();

	void SetBlockManager(BlockManager* blockManager) { blockManager_ = blockManager; }
	void SaveFile(uint32_t stageName);
	void LoadFile(uint32_t stageName);
	void Clear();
private:
	static const std::string kModelName;

	BlockManager* blockManager_;

	Vector3 rotate_;

	std::string fileName_;

	int stageIndex_;

	std::unique_ptr<ModelInstance> model_;
	bool isCreate_;
};