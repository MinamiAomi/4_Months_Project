#pragma once

#include <list>
#include <memory>

#include "Player/Player.h"
#include "FireBar.h"

class FireBarManager {
public:
	void Initialize(uint32_t stageIndex);
	void Update();
	void Reset(uint32_t stageIndex);

	void Create(const FireBar::Desc& desc);

	void SetPlayer(const Player* player) { player_ = player; }

	const std::list<std::unique_ptr<FireBar>>& GetFireBars() const { return fireBars_; }
	void DeleteFireBar(FireBar* block);

	void LoadJson(uint32_t stageIndex);

	void Clear();
private:
	const Player* player_;

	std::list<std::unique_ptr<FireBar>> fireBars_;
};