#include "DropGimmickManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"

void DropGimmickManager::Initialize() {
	dropperBallManager_ = std::make_unique<DropperBallManager>();
	dropperBallManager_->SetPlayer(player_);
	dropperBallManager_->SetBoss(boss_);
	dropperBallManager_->SetCamera(camera_);
	Clear();
}

void DropGimmickManager::Update() {
	for (auto& dropper : dropGimmicks_) {
		dropper->Update();
	}
	dropperBallManager_->Update();
}

void DropGimmickManager::Reset() {
	Clear();
}

void DropGimmickManager::Create(const DropGimmick::Desc& desc) {
	DropGimmick* dropGimmick = new DropGimmick();
	dropGimmick->SetCamera(camera_);
	dropGimmick->SetPlayer(player_);
	dropGimmick->SetBoss(boss_);
	dropGimmick->SetDropperBallManager(dropperBallManager_.get());
	dropGimmick->Initialize(desc);
	dropGimmicks_.emplace_back(std::move(dropGimmick));
}

void DropGimmickManager::Create(const DropperBall::Desc& desc) {
	dropperBallManager_->Create(desc);
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

void DropGimmickManager::Clear() {
	dropGimmicks_.clear();
	dropperBallManager_->Reset();
}
