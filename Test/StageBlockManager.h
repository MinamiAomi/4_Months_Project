#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Math/MathUtils.h"
#include "Graphics/Model.h"
#include "Boss/Boss.h"


class StageBlock;
class StageBlockManager :
	public GameObject {
public:

	static const int kAppearTime_ = 10;
	void Initialize();
	void Update();

	void SetBoss(const Boss* boss) { boss_ = boss; }
private:
	void UpdateTransform();
	void Appear();
	void DeadCheck();

	const Boss* boss_;

	std::list<std::unique_ptr<StageBlock>> stageBlocks_;
	int blockAppear_ = 0;
};