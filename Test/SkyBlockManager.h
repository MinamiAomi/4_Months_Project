#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Math/MathUtils.h"
#include "Graphics/Model.h"
#include "Boss/Boss.h"
#include "Math/Random.h"


class SkyBlock;
class SkyBlockManager :
	public GameObject {
public:

	static const int kBlockNum = 80;
	void Initialize();
	void Update();
	void Reset();

	void SetBoss(const Boss* boss) { boss_ = boss; }
private:
	void UpdateTransform();
	void Appear(SkyBlock& skyBlock);
	void DeadCheck();
	void DebugParam();

	const Boss* boss_;
	Random::RandomNumberGenerator rng_;

	std::array<std::unique_ptr<SkyBlock>, kBlockNum> skyBlocks_;
	float appearLength = 150.0f;
	Vector2 randRange_ = { 10.0f,7.0f };

};