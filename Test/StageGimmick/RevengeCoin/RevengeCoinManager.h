#pragma once

#include <list>
#include <memory>

#include "Player/Player.h"
#include "RevengeCoin.h"

class RevengeCoinManager {
public:
	void Initialize(uint32_t stageIndex);
	void Update();
	void Reset(uint32_t stageIndex);

	void Create(const StageGimmick::Desc& desc);

	void SetPlayer(const Player* player) { player_ = player; }

	const std::list<std::unique_ptr<RevengeCoin>>& GetBlocks() const { return revengeCoins_; }
	void Delete(RevengeCoin* revengeCoin);

	void LoadJson(uint32_t stageIndex);

	void Clear();

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Camera* camera_;
	const Player* player_;
	std::list<std::unique_ptr<RevengeCoin>> revengeCoins_;
};