#include "RevengeCoinManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"

void RevengeCoinManager::Initialize() {
	revengeCoins_.clear();
}

void RevengeCoinManager::Update() {
	for (auto& revengeCoin : revengeCoins_) {
		revengeCoin->Update();
	}
}

void RevengeCoinManager::Reset() {
	Clear();
}

void RevengeCoinManager::Create(const RevengeCoin::Desc& desc, uint32_t index) {
	RevengeCoin* revengeCoin = new RevengeCoin();
	revengeCoin->GetCamera(camera_);
	revengeCoin->SetPlayer(player_);
	revengeCoin->Initialize(desc);
	revengeCoin->stageGimmickNumber=index;
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

void RevengeCoinManager::Clear() {
	revengeCoins_.clear();
}
