#include "RevengeCoinManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"

void RevengeCoinManager::Initialize(uint32_t stageIndex) {
	revengeCoins_.clear();
	LoadJson(stageIndex);
}

void RevengeCoinManager::Update() {
	for (auto& revengeCoin : revengeCoins_) {
		revengeCoin->Update();
	}
}

void RevengeCoinManager::Reset(uint32_t stageIndex) {
	Clear();
	LoadJson(stageIndex);
}

void RevengeCoinManager::Create(const StageGimmick::Desc& desc) {
	RevengeCoin* revengeCoin = new RevengeCoin();
	revengeCoin->GetCamera(camera_);
	revengeCoin->SetPlayer(player_);
	revengeCoin->Initialize(desc);
	revengeCoins_.emplace_back(std::move(revengeCoin));
}

void RevengeCoinManager::Delete(RevengeCoin* revengeCoin) {
	// block がリスト内に存在するかを確認し、存在する場合はそのイテレータを取得する
	auto it = std::find_if(revengeCoins_.begin(), revengeCoins_.end(), [revengeCoin](const auto& ptr) {
		return ptr.get() == revengeCoin;
		});

	// block が見つかった場合は削除する
	if (it != revengeCoins_.end()) {
		revengeCoins_.erase(it);
	}
}

void RevengeCoinManager::LoadJson(uint32_t stageIndex) {
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
			obj["gimmick"]["type"] == "RevengeCoin") {
			Create(StageGimmick::GetDesc(obj));
		}
	}
}

void RevengeCoinManager::Clear() {
	revengeCoins_.clear();
}
