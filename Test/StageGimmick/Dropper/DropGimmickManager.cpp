#include "DropGimmickManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"

void DropGimmickManager::Initialize(uint32_t stageIndex) {
	dropGimmicks_.clear();
	LoadJson(stageIndex);
}

void DropGimmickManager::Update() {
	for (auto& dropper : dropGimmicks_) {
		dropper->Update();
	}
}

void DropGimmickManager::Reset(uint32_t stageIndex) {
	Clear();
	LoadJson(stageIndex);
}

void DropGimmickManager::Create(const DropGimmick::Desc& desc) {
	DropGimmick* dropGimmick = new DropGimmick();
	dropGimmick->SetCamera(camera_);
	dropGimmick->SetPlayer(player_);
	dropGimmick->SetBoss(boss_);
	dropGimmick->Initialize(desc);
	dropGimmicks_.emplace_back(std::move(dropGimmick));
}

void DropGimmickManager::Delete(DropGimmick* block) {
	// block がリスト内に存在するかを確認し、存在する場合はそのイテレータを取得する
	auto it = std::find_if(dropGimmicks_.begin(), dropGimmicks_.end(), [block](const auto& ptr) {
		return ptr.get() == block;
		});

	// block が見つかった場合は削除する
	if (it != dropGimmicks_.end()) {
		dropGimmicks_.erase(it);
	}
}

void DropGimmickManager::LoadJson(uint32_t stageIndex) {
	stageIndex;
	std::ifstream ifs(StageGimmick::stageScenePath_);
	if (!ifs.is_open()) {
		return;
	}

	// JSONをパースしてルートオブジェクトを取得
	nlohmann::json root;
	ifs >> root;
	ifs.close();

	std::vector<Switch::Desc> switchDesc{};
	std::vector<Dropper::Desc> dropperDesc{};
	// "objects"配列から"Block"オブジェクトを処理
	for (const auto& obj : root["objects"]) {
		if (obj.contains("gimmick") &&
			obj["gimmick"]["type"] == "Switch") {
			Switch::Desc desc{};
			desc.desc = StageGimmick::GetDesc(obj);
			const auto& gimmick = obj["gimmick"];
			desc.index = gimmick["index"];
			switchDesc.emplace_back(desc);
		}
		if (obj.contains("gimmick") &&
			obj["gimmick"]["type"] == "Dropper") {
			Dropper::Desc desc{};
			desc.desc = StageGimmick::GetDesc(obj);
			const auto& gimmick = obj["gimmick"];
			desc.index = gimmick["index"];
			dropperDesc.emplace_back(desc);
		}
	}
	// １ステージにおけるスイッチのタイプの数
	for (uint32_t i = 0; i < 10; i++) {
		DropGimmick::Desc dropGimmickDesc{};
		for (auto& desc : switchDesc) {
			if (desc.index == i) {
				dropGimmickDesc.switchDesc.emplace_back(desc);
			}
		}
		for (auto& desc : dropperDesc) {
			if (desc.index == i) {
				dropGimmickDesc.dropperDesc.emplace_back(desc);
			}
		}
		if (!dropGimmickDesc.switchDesc.empty()&&
			!dropGimmickDesc.dropperDesc.empty()) {
			Create(dropGimmickDesc);
		}
	}
}

void DropGimmickManager::Clear() {
	dropGimmicks_.clear();
}
