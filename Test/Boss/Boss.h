#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"

#include "Graphics/Model.h"
#include "BossModelManager/BossModelManager.h"
#include "BossStateManager/BossStateManager.h"
#include "BossHP/BossHP.h"
#include "BossUI/BossUI.h"

class Player;
class Camera;
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
	const bool GetIsAlive() const { return isAlive_; }

	const std::unique_ptr<BossModelManager>& GetModel() const { return bossModelManager_; }
	const std::unique_ptr<BossStateManager>& GetStateManager()const { return state_; }
	const std::unique_ptr<BossHP>& GetBossHP()const { return bossHP_; }
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	const Camera* camera_;
	const Player* player_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<BossModelManager> bossModelManager_;

	std::unique_ptr<BossStateManager> state_;
	
	std::unique_ptr<BossHP> bossHP_;

	std::unique_ptr<BossUI> bossUI_;

	Vector3 velocity_;
	Vector3 offset_;
	Vector3 easingStartPosition_;

	bool isMove_;
	bool isAlive_;
};