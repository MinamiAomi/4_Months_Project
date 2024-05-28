#include "BeltConveyorManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"


void BeltConveyorManager::Initialize() {
	beltConveyor_.clear();
}

void BeltConveyorManager::Update() {
	for (auto& beltConveyor : beltConveyor_) {
		beltConveyor->Update();
	}
}

void BeltConveyorManager::Reset() {
	Clear();
}

void BeltConveyorManager::Create(const BeltConveyor::Desc& desc, uint32_t index) {
	BeltConveyor* beltConveyor = new BeltConveyor();
	beltConveyor->stageGimmickNumber=index;
	beltConveyor->SetCamera(camera_);
	beltConveyor->SetPlayer(player_);
	beltConveyor->Initialize(desc);
	beltConveyor_.emplace_back(std::move(beltConveyor));
}

void BeltConveyorManager::Delete(BeltConveyor* beltConveyor) {
	// block がリスト内に存在するかを確認し、存在する場合はそのイテレータを取得する
	auto it = std::find_if(beltConveyor_.begin(), beltConveyor_.end(), [beltConveyor](const auto& ptr) {
		return ptr.get() == beltConveyor;
		});

	// block が見つかった場合は削除する
	if (it != beltConveyor_.end()) {
		beltConveyor_.erase(it);
	}
}

void BeltConveyorManager::Clear() {
	beltConveyor_.clear();
}
