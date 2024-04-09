#pragma once

#include <list>
#include <memory>

#include "Player/Player.h"
#include "Block.h"

class BlockManager {
public:
	void Initialize(uint32_t stageIndex);
	void Update();
	void Reset(uint32_t stageIndex);

	void Create(const Vector3& scale, const Vector3& rotate, const Vector3& position);

	void SetPlayer(const Player* player) { player_ = player; }

	const std::list<std::unique_ptr<Block>>& GetBlocks() const { return blocks_; }
	void DeleteBlock(Block* block);

	void LoadJson(uint32_t stageIndex);

	void Clear();
private:
	const Player* player_;
	std::list<std::unique_ptr<Block>> blocks_;
};