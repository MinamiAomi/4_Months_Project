#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"

#include "Graphics/Model.h"
#include "BossModelManager/BossModelManager.h"
#include "BossStateManager/BossStateManager.h"
#include "BossAttackTriggerManager/BossAttackTriggerManager.h"

class Player;
class Boss :
	public GameObject {
public:
	void Initialize();
	void Update();

	void Reset(uint32_t stageIndex);
	void SetIsMove(bool flag) { isMove_ = flag; }
	bool GetIsMove() { return isMove_; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
	void SetPlayer(const Player* player) { player_ = player; }


	const std::unique_ptr<BossModelManager>& GetModel() const { return bossModelManager_; }
	const std::unique_ptr<BossStateManager>& GetStateManager()const { return state_; }
	const std::unique_ptr<BossAttackTriggerManager>& GetAttackTriggerManager()const { return bossAttackTriggerManager_; }
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	const Camera* camera_;
	const Player* player_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<BossModelManager> bossModelManager_;

	std::unique_ptr<BossStateManager> state_;

	std::unique_ptr<BossAttackTriggerManager> bossAttackTriggerManager_;

	Vector3 velocity_;
	Vector3 offset_;
	Vector3 easingStartPosition_;

	bool isMove_;
};