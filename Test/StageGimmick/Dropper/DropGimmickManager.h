#pragma once

#include <list>
#include <memory>


#include "Player/Player.h"
#include "Boss/Boss.h"
#include "DropGimmick.h"

class DropGimmickManager {
public:
	void Initialize();
	void Update();
	void Reset();

	void Create(const DropGimmick::Desc& desc, uint32_t index);
	void Create(const DropperBall::Desc& desc, uint32_t index);

	std::list<std::unique_ptr<DropGimmick>>& GetDropGimmicks() { return dropGimmicks_; }
	std::unique_ptr<DropperBallManager>& GetDropperBallManager() { return dropperBallManager_; }
	void Delete(DropGimmick* block);

	void Clear();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
	void SetBoss(const Boss* boss) { boss_ = boss; }
private:
	const Camera* camera_;
	const Player* player_;
	const Boss* boss_;
	std::list<std::unique_ptr<DropGimmick>> dropGimmicks_;
	std::unique_ptr<DropperBallManager> dropperBallManager_;
};