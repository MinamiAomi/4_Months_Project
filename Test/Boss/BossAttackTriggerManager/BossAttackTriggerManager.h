#pragma once

#include <list>
#include <memory>

#include "BossAttackTrigger.h"

class BossAttackTriggerManager {
public:
	void Initialize();
	void Update();
	void Create(const BossAttackTrigger::Desc desc, uint32_t index);
	void Delete(BossAttackTrigger* bossAttackTrigger);
	void Reset();
	void Clear();

	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
	void SetModelIsAlive(bool flag);

	std::list<std::unique_ptr<BossAttackTrigger>>& GetBossAttackTriggers() { return bossAttackTriggers_; }
private:
	const Boss* boss_;
	const Camera* camera_;
	std::list<std::unique_ptr<BossAttackTrigger>> bossAttackTriggers_;
};