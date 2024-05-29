#include "FireBarManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"

void FireBarManager::Initialize() {
	Clear();
}

void FireBarManager::Update() {
	for (auto& fireBar : fireBars_) {
		fireBar->Update();
	}
}

void FireBarManager::Reset() {
	Clear();
}

void FireBarManager::Create(const FireBar::Desc& desc, uint32_t index) {
	FireBar* fireBar = new FireBar();
	fireBar->SetCamera(camera_);
	fireBar->SetPlayer(player_);
	fireBar->Initialize(desc);
	fireBar->stageGimmickNumber = index;
	fireBars_.emplace_back(std::move(fireBar));
}

void FireBarManager::DeleteFireBar(FireBar* block) {
	// block がリスト内に存在するかを確認し、存在する場合はそのイテレータを取得する
	auto it = std::find_if(fireBars_.begin(), fireBars_.end(), [block](const auto& ptr) {
		return ptr.get() == block;
		});

	// block が見つかった場合は削除する
	if (it != fireBars_.end()) {
		fireBars_.erase(it);
	}
}

void FireBarManager::Clear() {
	fireBars_.clear();
}
