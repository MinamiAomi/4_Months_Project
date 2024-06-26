#pragma once

#include <list>
#include <memory>

#include "BossAttackTrigger.h"

class BossAttackTriggerManager {
public:
	void Initialize(uint32_t stageIndex);
	void Update();
	void Create(const BossAttackTrigger::Desc desc);
	void Delete(BossAttackTrigger* bossAttackTrigger);
	void Reset(uint32_t stageIndex);
	void Clear();

	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
	void SetModelIsAlive(bool flag);
	void LoadJson(uint32_t stageIndex);

	const std::list<std::unique_ptr<BossAttackTrigger>>& GetBossAttackTriggers() const { return bossAttackTriggers_; }
private:
	const Boss* boss_;
	const Camera* camera_;
	std::list<std::unique_ptr<BossAttackTrigger>> bossAttackTriggers_;
};