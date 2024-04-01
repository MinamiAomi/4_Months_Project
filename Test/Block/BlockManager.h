#pragma once

#include <list>
#include <memory>

#include "Block.h"

class BlockManager {
public:
	void Initialize(uint32_t stageIndex);
	void Update();

	void Create(const Vector3& scale, const Vector3& rotate, const Vector3& position);

	const std::list<std::unique_ptr<Block>>& GetBlocks() const { return blocks_; }
	void DeleteBlock(Block* block);

	void LoadJson(uint32_t stageIndex);

	void Clear();
private:
	std::list<std::unique_ptr<Block>> blocks_;
};