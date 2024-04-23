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

class Player :
	public GameObject {
public:
	void Initialize();
	void Update();

	void Reset();

	const Vector3& GetLocalPos() const { return transform.translate; }
	const Matrix4x4& GetWorldMatrix() const { return transform.worldMatrix; }
	const Vector3& GetVelocity() const { return velocity_; }
	const Vector3& GetMinModelSize()const { return model_->GetModel()->GetMeshes()[0].minVertex; }
	const bool GetIsMove() const { return isMove_; }
	const bool GetIsGround() const { return isGround_; }

	void SetStageCamera(const StageCamera* stageCamera) { stageCamera_ = stageCamera; }
	void SetBoss(const Boss* boss) { boss_ = boss; }

	void SetIsMove(bool flag) { playerRevengeGage_->SetIsMove(flag); }
private:
	void Move();
	void Jump();
	void Invincible();
	void UpdateTransform();

	void OnCollision(const CollisionInfo& collisionInfo);

	const StageCamera* stageCamera_;
	const Boss* boss_;

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
#pragma endregion
#pragma region Json
	void DebugParam();
	void SaveJson();
	void LoadJson();
#pragma endregion
};