#pragma once

#include <list>
#include <memory>

#include "Player/Player.h"
#include "Block.h"

class BlockManager {
public:
	void Initialize();
	void Update();
	void Reset();

	void Create(const Block::Desc& desc, uint32_t index);

	void SetPlayer(const Player* player) { player_ = player; }

	std::list<std::unique_ptr<Block>>& GetBlocks() { return blocks_; }
	void DeleteBlock(Block* block);

	void Clear();

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Camera* camera_;
	const Player* player_;
	std::list<std::unique_ptr<Block>> blocks_;
};