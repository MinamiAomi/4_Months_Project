#pragma once

#include <list>
#include <memory>

#include "Player/Player.h"
#include "RevengeCoin.h"

class RevengeCoinManager {
public:
	void Initialize();
	void Update();
	void Reset();

	void Create(const RevengeCoin::Desc& desc);

	void SetPlayer(const Player* player) { player_ = player; }

	const std::list<std::unique_ptr<RevengeCoin>>& GetBlocks() const { return revengeCoins_; }
	void Delete(RevengeCoin* revengeCoin);
	void Clear();

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Camera* camera_;
	const Player* player_;
	std::list<std::unique_ptr<RevengeCoin>> revengeCoins_;
};