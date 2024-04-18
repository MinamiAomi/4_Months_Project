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
	for (auto& block : blocks_) {
		block->Update();
	}
}

void BlockManager::Reset(uint32_t stageIndex) {
	Clear();
	LoadJson(stageIndex);
}

void BlockManager::Create(const StageGimmick::Desc& desc) {
	Block* block = new Block();
	block->GetCamera(camera_);
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
		if (obj["type"] == "MESH") {
			StageGimmick::Desc desc{};
			desc.name = obj["file_name"];

			const auto& transform = obj["transform"];
			desc.transform.scale = { transform["scale"][0], transform["scale"][1], transform["scale"][2] };
			desc.transform.rotate = { transform["rotate"][0], transform["rotate"][1], transform["rotate"][2] };
			desc.transform.translate = { transform["translate"][0], transform["translate"][1], transform["translate"][2] };
			desc.collider.center = desc.transform.translate;

			const auto& collider = obj["collider"];
			desc.collider.center += { collider["center"][0], collider["center"][1], collider["center"][2] };
			desc.collider.rotate = { collider["rotate"][0], collider["rotate"][1], collider["rotate"][2] };
			desc.collider.size = { collider["size"][0], collider["size"][1], collider["size"][2] };

			Create(desc);
		}
	}
}


void BlockManager::Clear() {
	blocks_.clear();
}
