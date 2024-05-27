#pragma once

#include <list>

#include "StageObject.h"

class StageObjectManager {
public:
	void Initialize();
	void Update();
	void Reset();

	void Create(const StageObject::Desc& desc, uint32_t index);

	void SetPlayer(const Player* player) { player_ = player; }

	std::list<std::unique_ptr<StageObject>>& GetStageObjects() { return stageObjects_; }
	void Delete(StageObject* stageObject);

	void Clear();

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Camera* camera_;
	const Player* player_;

	std::list<std::unique_ptr<StageObject>> stageObjects_;
};