#include "FireBarManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"

void FireBarManager::Initialize(uint32_t stageIndex) {
	fireBars_.clear();
	LoadJson(stageIndex);
}

void FireBarManager::Update() {
	for (auto& fireBar : fireBars_) {
		fireBar->Update();
	}
}

void FireBarManager::Reset(uint32_t stageIndex) {
	Clear();
	LoadJson(stageIndex);
}

void FireBarManager::Create(const FireBar::Desc& desc) {
	FireBar* fireBar = new FireBar();
	fireBar->SetCamera(camera_);
	fireBar->SetPlayer(player_);
	fireBar->Initialize(desc);
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

void FireBarManager::LoadJson(uint32_t stageIndex) {
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
			obj["gimmick"]["type"] == "FireBar") {
			FireBar::Desc desc{};
			desc.desc = StageGimmick::GetDesc(obj);
			const auto& gimmick = obj["gimmick"];
			desc.barDesc.length = gimmick["length"];
			//desc.barDesc.barInitialAngle = gimmick["initialAngle"] * Math::ToRadian;
			//desc.barDesc.rotateVelocity = gimmick["rotateVelocity"];
			desc.barDesc.barInitialAngle = 0.0f;
			desc.barDesc.rotateVelocity = 0.1f;
			Create(desc);
		}
	}
}

void FireBarManager::Clear() {
	fireBars_.clear();
}
