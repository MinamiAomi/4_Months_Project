#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"

#include "Boss/Boss.h"
#include "CameraManager/StageCamera/StageCamera.h"
#include "Graphics/Model.h"
#include "Math/MathUtils.h"
#include "PlayerHP/PlayerHP.h"
#include "PlayerUI/PlayerUI.h"
#include "PlayerRevengGage/PlayerRevengeGage.h"
#include "PlayerBullet/BulletManager.h"
#include "PlayerHanmmer/PlayerHammer.h"
#include "Audio/AudioSource.h"
#include "Math/Random.h"

class TrapManager;
class Player :
	public GameObject {
public:
	void Initialize();
	void Update();
	void AnimationUpdate();
	void UpdateTransform();

	void SceneChangeUpdate();

	void Reset();

	void HitDamage(uint32_t damege = (uint32_t)- 1);

	PlayerRevengeGage* GetRevengeGage() { return playerRevengeGage_.get(); }
	const Vector3& GetLocalPos() const { return transform.translate; }
	Matrix4x4& GetWorldMatrix() { return transform.worldMatrix; }
	Transform& GetTransform() { return transform; }
	const Vector3& GetVelocity() const { return velocity_; }
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }
	ModelInstance& GetModelInstance() const { return *model_; }
	const Vector3& GetMinModelSize()const { return model_->GetModel()->GetMeshes()[0].minVertex; }
	const bool GetIsMove() const { return isMove_; }
	const bool GetIsGround() const { return isGround_; }
	const bool GetIsAlive() const { return isAlive_; }
	void SetBeltConveyorVelocity(float velocity) { beltConveyorVelocity_ = velocity; }

	void SetWindVelocity(const Vector3& velocity) { windVelocity_ = velocity; }
	void SetCamera(Camera* camera) { camera_ =camera; }
	void SetBoss(const Boss* boss) { boss_ = boss; }

	void SetIsMove(bool flag) { playerRevengeGage_->SetIsMove(flag); }
	const float GetChaseLimitLine() const {	return chaseLimitLine_;}
	const float GetRunAwayLimitLine() const {	return runAwayLimitLine_;}

	const Vector3& GetPrePos() const { return prePos_; }
	const Vector3& GetAcceleration() const { return acceleration_; }
	void SetTrapManager(TrapManager* trapManager) { trapManager_ = trapManager; }
private:
	enum AnimationType {
		IdleAnimation,
		MoveAnimation,
	};

	void Move();
	void Jump();
	void Dash();
	void Invincible();
	void SetTrap();

	void OnCollision(const CollisionInfo& collisionInfo);

	Camera* camera_;
	const Boss* boss_;
	TrapManager* trapManager_;
	
	Random::RandomNumberGenerator rnd_;

	std::unique_ptr<PlayerHP> playerHP_;
	std::unique_ptr<PlayerUI> playerUI_;
	std::unique_ptr<PlayerRevengeGage> playerRevengeGage_;
	std::unique_ptr<BulletManager> bulletManager_;

	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<PlayerHammer> hammer_;
	std::shared_ptr<Animation> animation_;
	std::shared_ptr<Skeleton> skeleton_;
	//PlayerModel playerModel_;
	std::unique_ptr<BoxCollider> collider_;
	Vector3 acceleration_;
	Vector3 velocity_;
	Vector3 prePos_;
	bool canFirstJump_;
	bool canSecondJump_;
	bool isMove_;
	bool isGround_;
	uint32_t invincibleTime_;
	bool isAlive_;
	Vector3 dashVector_;
	bool isDash_;
	uint32_t dashCount_;
	uint32_t dashCoolTime_;
	float beltConveyorVelocity_;
	float animationTime_;
	AnimationType animationType_;
	float fromTransitionAnimationTime_;
	bool animationTransition_;
	// ステージギミックにヒットした
	bool isHit_;
	bool preIsHit_;
	Vector3 windVelocity_;

	bool isSceneChangeInvincible_;
	// にダンジョンプ
	float rotateAnimationFrame_;
	Quaternion initialRotate_;
	Transform rotateAnimation_;
	bool isSecondJumpAnimation_;
	// JumpSE
	std::unique_ptr<AudioSource> firstJumpSE_;
	std::unique_ptr<AudioSource> secondJumpSE_;
	std::unique_ptr<AudioSource> revengeSE_;
	std::unique_ptr<AudioSource> damageSE_;
	std::unique_ptr<AudioSource> onGroundSE_;
#pragma region パラメーター
	float verticalSpeed_;
	float horizontalSpeed_;
	float jumpPower_;
	float gravity_;
	float chaseLimitLine_;
	float runAwayLimitLine_;
	float knockBack_;
	uint32_t maxInvincibleTime_;
	Vector3 offset_;
	float hitJump_;
	float dashPower_;
	uint32_t dashMaxCount_;
	uint32_t dashIntervalCount_;
	float rotateAnimationAllFrame_;
#pragma endregion

#pragma region Json
	void DebugParam();
#pragma endregion
};