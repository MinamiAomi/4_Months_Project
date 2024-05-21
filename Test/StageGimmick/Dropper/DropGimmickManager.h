#pragma once

#include <list>
#include <memory>


#include "Player/Player.h"
#include "Boss/Boss.h"
#include "DropGimmick.h"

class DropGimmickManager {
public:
	void Initialize(uint32_t stageIndex);
	void Update();
	void Reset(uint32_t stageIndex);

	void Create(const DropGimmick::Desc& desc);

	const std::list<std::unique_ptr<DropGimmick>>& GetDropGimmicks() const { return dropGimmicks_; }
	void Delete(DropGimmick* block);

	void LoadJson(uint32_t stageIndex);

	void Clear();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
	void SetBoss(const Boss* boss) { boss_ = boss; }
private:
	const Camera* camera_;
	const Player* player_;
	const Boss* boss_;
	std::list<std::unique_ptr<DropGimmick>> dropGimmicks_;
};