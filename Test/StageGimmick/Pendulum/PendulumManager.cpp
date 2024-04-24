#include "PendulumManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"

void PendulumManager::Initialize(uint32_t stageIndex) {
	Clear();
	LoadJson(stageIndex);
}

void PendulumManager::Update() {
	for (auto& pendulum : pendulums_) {
		pendulum->Update();
	}
}

void PendulumManager::Reset(uint32_t stageIndex) {
	Clear();
	LoadJson(stageIndex);
}

void PendulumManager::Create(Pendulum::Desc desc) {
	Pendulum* pendulum = new Pendulum();
	pendulum->SetCamera(camera_);
	pendulum->SetPlayer(player_);
	pendulum->Initialize(desc);
	pendulums_.emplace_back(std::move(pendulum));
}

void PendulumManager::DeletePendulum(Pendulum* block) {
	// block がリスト内に存在するかを確認し、存在する場合はそのイテレータを取得する
	auto it = std::find_if(pendulums_.begin(), pendulums_.end(), [block](const auto& ptr) {
		return ptr.get() == block;
		});

	// block が見つかった場合は削除する
	if (it != pendulums_.end()) {
		pendulums_.erase(it);
	}
}

void PendulumManager::LoadJson(uint32_t stageIndex) {
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
			obj["gimmick"]["type"] == "Pendulum") {
			Pendulum::Desc desc{};
			desc.desc = StageGimmick::GetDesc(obj);
			const auto& gimmick = obj["gimmick"];
			desc.length = gimmick["length"];
			desc.angle = gimmick["angle"] * Math::ToRadian;
			desc.initializeAngle = gimmick["initializeAngle"] * Math::ToRadian;
			desc.gravity = gimmick["gravity"];
			desc.stickScale = gimmick["stickScale"];
			desc.ballScale = gimmick["ballScale"];
			Create(desc);
		}
	}
}

void PendulumManager::Clear() {
	pendulums_.clear();
}
