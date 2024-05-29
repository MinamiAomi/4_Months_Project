#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"

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
	void UpdateTransform();

	void Reset(uint32_t stageIndex);
	void SetIsMove(bool flag) { isMove_ = flag; }
	bool GetIsMove() { return isMove_; }
	bool GetIsFirstHit() const  { return isFirstHit_; }
	void SetIsEndFirstHitMovie(bool flag) { isEndFirstHitMovie_ = flag; }
	void SetCamera(Camera* camera) { camera_ = camera; }
	void SetPlayer(Player* player) { player_ = player; }
	const bool GetIsAlive() const { return isAlive_; }

	const std::unique_ptr<BossModelManager>& GetModelManager() const { return bossModelManager_; }
	const std::unique_ptr<BossStateManager>& GetStateManager()const { return state_; }
	const std::unique_ptr<BossHP>& GetBossHP()const { return bossHP_; }
private:
	void OnCollision(const CollisionInfo& collisionInfo);

	Camera* camera_;
	Player* player_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<BossModelManager> bossModelManager_;

	std::unique_ptr<BossStateManager> state_;
	
	std::unique_ptr<BossHP> bossHP_;

	std::unique_ptr<BossUI> bossUI_;

	Vector3 velocity_;
	Vector3 offset_;
	Vector3 easingStartPosition_;

	Vector3 toCameraVector_;

	bool isMove_;
	bool isAlive_;
	bool isFirstHit_;
	bool isEndFirstHitMovie_;
};