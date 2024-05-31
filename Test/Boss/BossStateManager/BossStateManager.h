#pragma once
#include "Collision/Collider.h"

#include <memory>
#include <array>

#include "Graphics/Skeleton.h"
#include "Math/MathUtils.h"
#include "Math/Random.h"

class Player;
class Boss;
class BossState;
struct CollisionInfo;
class BossStateManager;
class BossState {
public:
	enum AttackState {
		kRotate,
		kChage,
		kAttack,
	};
	BossState(BossStateManager& manager, bool inTransition) : manager_(manager), inTransition_(inTransition) {}
	virtual ~BossState() {}
	virtual void Initialize() = 0;
	virtual	void SetDesc() = 0;
	virtual void Update() = 0;
	virtual void OnCollision(const CollisionInfo& collisionInfo) = 0;
	virtual AnimationSet* GetAnimation() const = 0;
	virtual float GetAnimationTime() const = 0;
	BossStateManager& GetManager() { return manager_; }
protected:
	BossStateManager& manager_;
	AttackState attackState_;
	float time_;
	Random::RandomNumberGenerator rnd_;
	bool inTransition_;
};

class BossStateRoot :
	public BossState {
public:
	struct JsonData {
		float allFrame;
		float transitionFrame;
	};
	using BossState::BossState;
	void Initialize() override;
	void SetDesc() override;
	void Update() override;
	void OnCollision(const CollisionInfo& collisionInfo) override;
	AnimationSet* GetAnimation() const override;
	float GetAnimationTime() const override;

private:
	JsonData data_;
};

class BossStateHook :
	public BossState {
public:
	struct JsonData {
		float allFrame;
		float transitionFrame;
	};
	using BossState::BossState;
	void Initialize() override;
	void SetDesc() override;
	void Update() override;
	void OnCollision(const CollisionInfo& collisionInfo) override;
	AnimationSet* GetAnimation() const override;
	float GetAnimationTime() const override;
private:
	JsonData data_;
	
};

class BossStateLowerAttack :
	public BossState {
public:
	struct JsonData {
		Vector3 position;
		Vector3 scale;
		float attackEasingTime;
		float chargeEasingTime;
		float transitionFrame;
	};
	using BossState::BossState;
	void Initialize() override;
	void SetDesc() override;
	void Update() override;
	void OnCollision(const CollisionInfo& collisionInfo) override;
	AnimationSet* GetAnimation() const override;
	float GetAnimationTime() const override;
private:
	void ChargeUpdate();
	void AttackUpdate();
	JsonData data_;
	float time_;
};

class BossStateInsideAttack :
	public BossState {
public:
	struct JsonData {
		Vector3 position;
		Vector3 scale;
		float attackEasingTime;
		float chargeEasingTime;
		float transitionFrame;
	};
	using BossState::BossState;
	void Initialize() override;
	void SetDesc() override;
	void Update() override;
	void OnCollision(const CollisionInfo& collisionInfo) override;
	AnimationSet* GetAnimation() const override;
	float GetAnimationTime() const override;
private:
	void ChargeUpdate();
	void AttackUpdate();
	JsonData data_;
	float time_;
};

class BossStateBeamAttack:
	public BossState {
public:
	struct JsonData {
		Vector3 position;
		Vector3 scale;
		Vector3 vector;
		float rotateEasingTime;
		float attackEasingTime;
		float chargeEasingTime;
		float transitionFrame;

		struct Wind {
			struct MinMax {
				float min;
				float max;
			};
			Vector3 offset;
			MinMax velocity;
			MinMax startScale;
			MinMax endScale;
			float rotate;
		}wind;
	};
	using BossState::BossState;
	void Initialize() override;
	void SetDesc() override;
	void Update() override;
	void OnCollision(const CollisionInfo& collisionInfo) override;
	AnimationSet* GetAnimation() const override;
	float GetAnimationTime() const override;
private:
	void ChargeUpdate();
	void AttackUpdate();
	void RotateUpdate();
	JsonData data_;
	float time_;
	float lastWindTime_;
};

class BossStateShotAttack :
	public BossState {
public:
	struct JsonData {
		Vector3 offset;
		float velocity;
		float range;
		float attackEasingTime;
		float chargeEasingTime;
		float rotateEasingTime;
		uint32_t numBullet;
		float transitionFrame;
	};
	using BossState::BossState;
	void Initialize() override;
	void SetDesc() override;
	void Update() override;
	void OnCollision(const CollisionInfo& collisionInfo) override;
	AnimationSet* GetAnimation() const override;
	float GetAnimationTime() const override;
private:
	void ChargeUpdate();
	void AttackUpdate();
	void RotateUpdate();
	JsonData data_;
	float time_;
	float lastBulletTime_;
};


class BossStateManager {
public:
	enum State {
		kRoot,
		kHook,
		kInsideAttack,
		kLowerAttack,
		kBeamAttack,
		kShotAttack,
	};

	struct JsonData {
		BossStateRoot::JsonData rootData;
		BossStateHook::JsonData attackData;
		BossStateLowerAttack::JsonData lowerAttackData;
		BossStateInsideAttack::JsonData insideAttackData;
		BossStateBeamAttack::JsonData beamAttackData;
		BossStateShotAttack::JsonData shotAttackData;
	};
	BossStateManager(Boss& boss) : boss(boss) {}
	void Initialize();
	void Update();
	void OnCollision(const CollisionInfo& collisionInfo);
	void DrawImGui();

	const State GetState()const { return state_; }

	const JsonData& GetData() { return jsonData_; }
	void ChangeState(const BossStateManager::State& state);
	AnimationSet* GetPrevAnimation() const { return prevAnimation_; }
	float GetPrevAnimationTime() const { return prevAnimationTime_; }

	Boss& boss;
	JsonData jsonData_;
private:
	std::unique_ptr<BossState> activeState_;
	std::unique_ptr<BossState> standbyState_;
	AnimationSet* prevAnimation_ = nullptr;
	float prevAnimationTime_;
	State state_;
};