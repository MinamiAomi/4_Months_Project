#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Math/MathUtils.h"
#include "Graphics/Model.h"
#include "Boss/Boss.h"
#include "Math/Random.h"


class StageBlock;
class StageBlockManager :
	public GameObject {
public:

	static const int kAppearTime_ = 15;
	void Initialize();
	void Update();
	void Reset();

	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetIsMove(bool isMove) { isMove_ = isMove; }
private:
	void UpdateTransform();
	void Appear();
	void DeadCheck();

	const Boss* boss_;
	Random::RandomNumberGenerator rng_;

	std::list<std::unique_ptr<StageBlock>> stageBlocks_;
	int blockAppear_ = 0;
	bool isMove_ = true;
};