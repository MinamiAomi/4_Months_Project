#include "BossAttackTriggerManager.h"

#include <fstream>

#include "Externals/nlohmann/json.hpp"
#include "StageGimmick/StageGimmick.h"

void BossAttackTriggerManager::Initialize(uint32_t stageIndex) {
	bossAttackTriggers_.clear();
	LoadJson(stageIndex);
}

void BossAttackTriggerManager::Update() {
	for (auto& trigger : bossAttackTriggers_) {
		trigger->Update();
	}
}

void BossAttackTriggerManager::Create(const BossAttackTrigger::Desc desc) {
	BossAttackTrigger* bossAttackTrigger = new BossAttackTrigger();
	bossAttackTrigger->SetCamera(camera_);
	bossAttackTrigger->SetBoss(boss_);
	bossAttackTrigger->Initialize(desc);
	bossAttackTriggers_.emplace_back(std::move(bossAttackTrigger));
}

void BossAttackTriggerManager::Delete(BossAttackTrigger* bossAttackTrigger) {
	// bossAttackTriggers_ がリスト内に存在するかを確認し、存在する場合はそのイテレータを取得する
	auto it = std::find_if(bossAttackTriggers_.begin(), bossAttackTriggers_.end(), [bossAttackTrigger](const auto& ptr) {
		return ptr.get() == bossAttackTrigger;
		});

	// block が見つかった場合は削除する
	if (it != bossAttackTriggers_.end()) {
		bossAttackTriggers_.erase(it);
	}
}

void BossAttackTriggerManager::Reset(uint32_t stageIndex) {
	Clear();
	LoadJson(stageIndex);
}

void BossAttackTriggerManager::Clear() {
	bossAttackTriggers_.clear();
}

void BossAttackTriggerManager::SetModelIsAlive(bool flag) {
	for (auto& trigger : bossAttackTriggers_) {
		trigger->SetIsModelAlive(flag);
	}
}

void BossAttackTriggerManager::LoadJson(uint32_t stageIndex) {
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
			obj["gimmick"]["type"] == "BossAttackTrigger") {
			BossAttackTrigger::Desc desc{};
			const auto& gimmick = obj["gimmick"];
			desc.pos = gimmick["position"];
			desc.state = static_cast<BossStateManager::State>(gimmick["state"]);
			Create(desc);
		}
	}
}
