#include "FloorManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"

void FloorManager::Initialize() {
	Clear();
}

void FloorManager::Update() {
	for (auto& floor : floors_) {
		floor->Update();
	}
}

void FloorManager::Reset() {
	Clear();
}

void FloorManager::Create(const Floor::Desc& desc, uint32_t index) {
	Floor* floor = new Floor();
	floor->SetCamera(camera_);
	floor->SetPlayer(player_);
	floor->Initialize(desc);
	floor->stageGimmickNumber=index;
	floors_.emplace_back(std::move(floor));
}

void FloorManager::DeleteFloor(Floor* block) {
	// block がリスト内に存在するかを確認し、存在する場合はそのイテレータを取得する
	auto it = std::find_if(floors_.begin(), floors_.end(), [block](const auto& ptr) {
		return ptr.get() == block;
		});

	// block が見つかった場合は削除する
	if (it != floors_.end()) {
		floors_.erase(it);
	}
}


void FloorManager::Clear() {
	floors_.clear();
}
