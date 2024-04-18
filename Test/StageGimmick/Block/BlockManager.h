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

	void Create(const StageGimmick::Desc& desc);

	void SetPlayer(const Player* player) { player_ = player; }

	const std::list<std::unique_ptr<Block>>& GetBlocks() const { return blocks_; }
	void DeleteBlock(Block* block);

	void LoadJson(uint32_t stageIndex);

	void Clear();

	void SetCamara(const Camera* camera) { camera_ = camera; }
private:
	const Camera* camera_;
	const Player* player_;
	std::list<std::unique_ptr<Block>> blocks_;
};