#include "PendulumManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"

void PendulumManager::Initialize() {
	Clear();
}

void PendulumManager::Update() {
	for (auto& pendulum : pendulums_) {
		pendulum->Update();
	}
}

void PendulumManager::Reset() {
	Clear();
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

void PendulumManager::Clear() {
	pendulums_.clear();
}
