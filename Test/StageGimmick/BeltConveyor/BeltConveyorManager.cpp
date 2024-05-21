#include "BeltConveyorManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"


void BeltConveyorManager::Initialize(uint32_t stageIndex) {
	beltConveyor_.clear();
	LoadJson(stageIndex);
}

void BeltConveyorManager::Update() {
	for (auto& beltConveyor : beltConveyor_) {
		beltConveyor->Update();
	}
}

void BeltConveyorManager::Reset(uint32_t stageIndex) {
	Clear();
	LoadJson(stageIndex);
}

void BeltConveyorManager::Create(const BeltConveyor::Desc& desc) {
	BeltConveyor* beltConveyor = new BeltConveyor();
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

void BeltConveyorManager::LoadJson(uint32_t stageIndex) {
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
			obj["gimmick"]["type"] == "BeltConveyor") {
			BeltConveyor::Desc desc{};
			desc.desc = StageGimmick::GetDesc(obj);
			const auto& gimmick = obj["gimmick"];
			desc.velocity = gimmick["velocity"];
			Create(desc);
		}
	}
}

void BeltConveyorManager::Clear() {
	beltConveyor_.clear();
}
