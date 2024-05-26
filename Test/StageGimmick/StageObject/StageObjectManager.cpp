#include "StageObjectManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>

#include "Externals/nlohmann/json.hpp"

void StageObjectManager::Initialize() {
	stageObjects_.clear();
}

void StageObjectManager::Update() {
	for (auto& stageObject : stageObjects_) {
		stageObject->Update();
	}
}

void StageObjectManager::Reset() {
	Clear();
}

void StageObjectManager::Create(const StageObject::Desc& desc) {
	StageObject* stageObject = new StageObject();
	stageObject->SetCamera(camera_);
	stageObject->Initialize(desc);
	stageObjects_.emplace_back(std::move(stageObject));
}

void StageObjectManager::Delete(StageObject* stageObject) {
	// block がリスト内に存在するかを確認し、存在する場合はそのイテレータを取得する
	auto it = std::find_if(stageObjects_.begin(), stageObjects_.end(), [stageObject](const auto& ptr) {
		return ptr.get() == stageObject;
		});

	// block が見つかった場合は削除する
	if (it != stageObjects_.end()) {
		stageObjects_.erase(it);
	}
}

void StageObjectManager::Clear() {
	stageObjects_.clear();
}
