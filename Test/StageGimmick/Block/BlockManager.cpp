#include "BlockManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"

void BlockManager::Initialize(uint32_t stageIndex) {
	blocks_.clear();
	LoadJson(stageIndex);
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

void BlockManager::Reset(uint32_t stageIndex) {
	Clear();
	LoadJson(stageIndex);
}

void BlockManager::Create(const Block::Desc& desc) {
	Block* block = new Block();
	block->SetCamera(camera_);
	block->SetPlayer(player_);
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

void BlockManager::LoadJson(uint32_t stageIndex) {
	stageIndex;
	std::ifstream ifs(StageGimmick::stageScenePath_);
	if (!ifs.is_open()) {
		return;
	}

	// JSONをパースしてルートオブジェクトを取得
	nlohmann::json root;
	ifs >> root;
	ifs.close();

	// "objects"配列から"Block"オブジェクトを処理
	for (const auto& obj : root["objects"]) {
		if (obj.contains("gimmick") &&
			obj["gimmick"]["type"] == "Block") {
			Block::Desc desc{};
			desc.desc = StageGimmick::GetDesc(obj);
			Create(desc);
		}
	}
}


void BlockManager::Clear() {
	blocks_.clear();
}
