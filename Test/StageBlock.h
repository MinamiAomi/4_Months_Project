#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Math/MathUtils.h"
#include "Graphics/Model.h"
#include "Boss/Boss.h"

#include "Graphics/LightManager.h"
#include "CharacterState.h"

class StageBlock :
	public GameObject {
public:

	static const int kAppearFrame = 20;

	StageBlock();
	~StageBlock();
	void Initialize(const Vector3& pos, const Vector3& direction);
	void Update();

	void SetBoss(const Boss* boss) { boss_ = boss; }
	bool GetIsDead() { return isDead_; }
private:
	void UpdateTransform();

	const Boss* boss_;

	std::unique_ptr<ModelInstance> model_;
	Vector3 direction_;

	int appearFrame_ = kAppearFrame;
	bool isDead_ = false;
	Character::State initialState_;
	
};