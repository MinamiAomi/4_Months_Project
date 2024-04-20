#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Math/MathUtils.h"
#include "Graphics/Model.h"
#include "Boss/Boss.h"

#include "Graphics/LightManager.h"

class SkyBlock :
	public GameObject {
public:


	SkyBlock();
	~SkyBlock();
	void Initialize(const Vector3& pos, const Vector3& scale);
	void Update(float apperLength);

	void SetBoss(const Boss* boss) { boss_ = boss; }
	bool GetIsDead() { return isDead_; }
private:
	void UpdateTransform();

	const Boss* boss_;

	std::unique_ptr<ModelInstance> model_;

	bool isDead_ = false;
	
};