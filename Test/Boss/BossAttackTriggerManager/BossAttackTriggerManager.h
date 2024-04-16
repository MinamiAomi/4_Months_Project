#pragma once

#include <list>
#include <memory>

#include "BossAttackTrigger.h"

class BossAttackTriggerManager {
public:
	void Initialize();
	void Update();
	void Create(const BossAttackTrigger::Desc desc);
	void Delete(BossAttackTrigger* bossAttackTrigger);
	void Clear();

	void SetBoss(const Boss* boss) { boss_ = boss; }
	void LoadJson(uint32_t stageIndex);
	const std::list<std::unique_ptr<BossAttackTrigger>>& GetBossAttackTriggers() const { return bossAttackTriggers_; }
private:
	const Boss* boss_;
	std::list<std::unique_ptr<BossAttackTrigger>> bossAttackTriggers_;
};