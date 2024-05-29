#pragma once

#include <memory>
#include <string>

#include "Boss/BossAttackTriggerManager/BossAttackTriggerManager.h"
#include "Boss/BossAttackTriggerManager/BossAttackTrigger.h"
#include "Boss/Boss.h"

class BossAttackTriggerEditor {
public:
	void Initialize();
	void Update();

	void SetBossAttackTriggerManager(BossAttackTriggerManager* bossAttackTriggerManager) { bossAttackTriggerManager_ = bossAttackTriggerManager; }
	void SaveFile(uint32_t stageName);
	void Clear();

	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetCamera(const Camera* camera) { camera_ = camera; }

private:
	const Boss* boss_;
	const Camera* camera_;

	BossAttackTriggerManager* bossAttackTriggerManager_;

	std::unique_ptr<BossAttackTrigger> bossAttackTrigger_;

	std::string fileName_;

	int stageIndex_;

	bool isCreate_;
	bool isAlive_;
};