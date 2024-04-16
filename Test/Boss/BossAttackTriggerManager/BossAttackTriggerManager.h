#pragma once

#include <list>
#include <memory>

#include "BossAttackTrigger.h"
#include "Boss/Boss.h"

class BossAttackTriggerManager {
public:
	void Initialize();
	void Update();
	void Create(const BossAttackTrigger::Desc desc);
	void Delete(BossAttackTrigger* bossAttackTrigger);

	void SetBoss(const Boss* boss) { boss_ = boss; }
	void LoadJson(uint32_t stageIndex);
private:
	const Boss* boss_;
	std::list<std::unique_ptr<BossAttackTrigger>> bossAttackTriggers_;
};