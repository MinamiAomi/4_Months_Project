#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"

#include "Boss/Boss.h"
#include "CameraManager/StageCamera/StageCamera.h"
#include "Graphics/Model.h"
#include "Math/MathUtils.h"
#include "PlayerModel.h"
#include "PlayerHP/PlayerHP.h"
#include "PlayerUI/PlayerUI.h"
#include "PlayerRevengGage/PlayerRevengeGage.h"
#include "PlayerBullet/BulletManager.h"
#include "Engine/Audio/AudioSource.h"
#include "Engine/Math/Random.h"

class TrapManager;
class Player :
	public GameObject {
public:
	void Initialize();
	void Update();

	void Reset();

	const PlayerRevengeGage& GetRevengeGage() const { return *playerRevengeGage_.get(); }
	const Vector3& GetLocalPos() const { return transform.translate; }
	const Matrix4x4& GetWorldMatrix() const { return transform.worldMatrix; }
	const Vector3& GetVelocity() const { return velocity_; }
	const Vector3& GetMinModelSize()const { return model_->GetModel()->GetMeshes()[0].minVertex; }
	const bool GetIsMove() const { return isMove_; }
	const bool GetIsGround() const { return isGround_; }
	const bool GetIsAlive() const { return isAlive_; }

	void SetStageCamera(const StageCamera* stageCamera) { stageCamera_ = stageCamera; }
	void SetBoss(const Boss* boss) { boss_ = boss; }

	void SetIsMove(bool flag) { playerRevengeGage_->SetIsMove(flag); }
	const float GetChaseLimitLine() const {	return chaseLimitLine_;}

	void SetTrapManager(TrapManager* trapManager) { trapManager_ = trapManager; }
private:
	void Move();
	void Jump();
	void Dash();
	void Invincible();
	void SetTrap();
	void UpdateTransform();

	void OnCollision(const CollisionInfo& collisionInfo);

	const StageCamera* stageCamera_;
	const Boss* boss_;
	TrapManager* trapManager_;
	
	Random::RandomNumberGenerator rnd_;

	std::unique_ptr<PlayerHP> playerHP_;
	std::unique_ptr<PlayerUI> playerUI_;
	std::unique_ptr<PlayerRevengeGage> playerRevengeGage_;
	std::unique_ptr<BulletManager> bulletManager_;

	std::unique_ptr<ModelInstance> model_;
	//PlayerModel playerModel_;
	std::unique_ptr<BoxCollider> collider_;

	Vector3 acceleration_;
	Vector3 velocity_;
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
	// ステージギミックにヒットした
	bool isHit_;
	bool preIsHit_;

	// JumpSE
	std::unique_ptr<AudioSource> jumpSE_;
	std::unique_ptr<AudioSource> revengeSE_;
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
	Vector3 revengeStartOffset_;
	float hitJump_;
	float dashPower_;
	uint32_t dashMaxCount_;
	uint32_t dashIntervalCount_;
#pragma endregion
#pragma region Json
	void DebugParam();
	void SaveJson();
	void LoadJson();
#pragma endregion
};