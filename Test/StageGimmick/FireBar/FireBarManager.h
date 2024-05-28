#pragma once

#include <list>
#include <memory>

#include "Player/Player.h"
#include "FireBar.h"

class FireBarManager {
public:
	void Initialize();
	void Update();
	void Reset();

	void Create(const FireBar::Desc& desc, uint32_t index);

	void SetPlayer(const Player* player) { player_ = player; }

	std::list<std::unique_ptr<FireBar>>& GetFireBars() { return fireBars_; }
	void DeleteFireBar(FireBar* block);

	void Clear();

	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	const Player* player_;
	const Camera* camera_;

	std::list<std::unique_ptr<FireBar>> fireBars_;
};