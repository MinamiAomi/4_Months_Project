#include "BlockManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"

void BlockManager::Initialize() {
	blocks_.clear();
}

void BlockManager::Update() {
	for (auto it = blocks_.begin(); it != blocks_.end(); ) {
		(*it)->Update();
		if (!(*it)->GetIsAlive()) {
			it = blocks_.erase(it);
		}
		else {
			++it;
		}
	}
}

void BlockManager::Reset() {
	Clear();
}

void BlockManager::Create(const Block::Desc& desc,uint32_t index) {
	Block* block = new Block();
	block->SetCamera(camera_);
	block->SetPlayer(player_);
	block->stageGimmickNumber = index;
	block->Initialize(desc);
	blocks_.emplace_back(std::move(block));
}

void BlockManager::DeleteBlock(Block* block) {
	// block がリスト内に存在するかを確認し、存在する場合はそのイテレータを取得する
	auto it = std::find_if(blocks_.begin(), blocks_.end(), [block](const auto& ptr) {
		return ptr.get() == block;
		});

	// block が見つかった場合は削除する
	if (it != blocks_.end()) {
		blocks_.erase(it);
	}
}



void BlockManager::Clear() {
	blocks_.clear();
}
