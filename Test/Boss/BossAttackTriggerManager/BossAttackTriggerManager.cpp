#include "BossAttackTriggerManager.h"

#include <fstream>

#include "Externals/nlohmann/json.hpp"
#include "StageGimmick/StageGimmick.h"
#include "CharacterState.h"

void BossAttackTriggerManager::Initialize() {
	bossAttackTriggers_.clear();
}

void BossAttackTriggerManager::Update() {
	switch (Character::currentCharacterState_) {
	case Character::State::kChase:
	{

	}
	break;
	case Character::State::kRunAway:
	{
		switch (Character::nextCharacterState_) {
		case Character::State::kChase:
		{

		}
		break;
		case Character::State::kRunAway:
		{
			for (auto& trigger : bossAttackTriggers_) {
				trigger->Reset();
			}
		}
		break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}
	for (auto& trigger : bossAttackTriggers_) {
		trigger->Update();
	}
}

void BossAttackTriggerManager::Create(const BossAttackTrigger::Desc desc , uint32_t index) {
	BossAttackTrigger* bossAttackTrigger = new BossAttackTrigger();
	bossAttackTrigger->SetCamera(camera_);
	bossAttackTrigger->SetBoss(boss_);
	bossAttackTrigger->Initialize(desc);
	bossAttackTrigger->stageGimmickNumber=index;
	bossAttackTriggers_.emplace_back(std::move(bossAttackTrigger));
}

void BossAttackTriggerManager::Delete(BossAttackTrigger* bossAttackTrigger) {
	// bossAttackTriggers_ がリスト内に存在するかを確認し、存在する場合はそのイテレータを取得する
	auto it = std::find_if(bossAttackTriggers_.begin(), bossAttackTriggers_.end(), [bossAttackTrigger](const auto& ptr) {
		return ptr.get() == bossAttackTrigger;
		});

	// block が見つかった場合は削除する
	if (it != bossAttackTriggers_.end()) {
		bossAttackTriggers_.erase(it);
	}
}

void BossAttackTriggerManager::Reset() {
	Clear();
}

void BossAttackTriggerManager::Clear() {
	bossAttackTriggers_.clear();
}

void BossAttackTriggerManager::SetModelIsAlive(bool flag) {
	for (auto& trigger : bossAttackTriggers_) {
		trigger->SetIsModelAlive(flag);
	}
}
