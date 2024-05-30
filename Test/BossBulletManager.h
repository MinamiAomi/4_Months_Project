#pragma once

#include <list>
#include <memory>

#include "Math/MathUtils.h"
#include "BossBullet.h"

class BossBulletManager {
public:
	// ボスのステートマネージャーに渡すのがガガガ
	static BossBulletManager* GetInstance();
	BossBulletManager(const BossBulletManager&) = delete;
	BossBulletManager& operator=(const BossBulletManager&) = delete;
	void Initialize();
	void Update();
	void Create(const Transform& parent,const Vector3& pos,const Vector3& velocity);
	void SetVelocity(uint32_t index, float velocity);
	void Reset();
	std::list<std::unique_ptr<BossBullet>>& GetBullets() { return bullets_; }
private:
	BossBulletManager() = default;
	~BossBulletManager() = default;
	std::list<std::unique_ptr<BossBullet>> bullets_;
};