#pragma once
#include "Collision/Collider.h"

#include <memory>
#include <array>

#include "Graphics/Skeleton.h"
#include "Math/MathUtils.h"

class Boss;
class BossState;
struct CollisionInfo;
class BossStateManager;
class BossState {
public:
	enum AttackState {
		kChage,
		kAttack,
	};
	BossState(BossStateManager& manager) : manager_(manager) {}
	virtual ~BossState() {}
	virtual void Initialize() = 0;
	virtual	void SetDesc() = 0;
	virtual void Update() = 0;
	virtual void OnCollision(const CollisionInfo& collisionInfo) = 0;
	BossStateManager& GetManager() { return manager_; }
protected:
	BossStateManager& manager_;
	AttackState attackState_;
};

class BossStateRoot :
	public BossState {
public:
	struct JsonData {
		float velocity;
	};
	using BossState::BossState;
	void Initialize() override;
	void SetDesc() override;
	void Update() override;
	void OnCollision(const CollisionInfo& collisionInfo) override;

private:
	float velocity_;
};

class BossStateHook :
	public BossState {
public:
	struct JsonData {
		float allFrame;
	};
	using BossState::BossState;
	void Initialize() override;
	void SetDesc() override;
	void Update() override;
	void OnCollision(const CollisionInfo& collisionInfo) override;
private:
	JsonData data_;
	float time_;
};

class BossStateLowerAttack :
	public BossState {
public:
	struct JsonData {
		Vector3 startPosition;
		Vector3 endPosition;
		Vector3 scale;
		float attackEasingTime;
		float chargeEasingTime;
		float velocity;
	};
	using BossState::BossState;
	void Initialize() override;
	void SetDesc() override;
	void Update() override;
	void OnCollision(const CollisionInfo& collisionInfo) override;
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
		Vector3 startPosition;
		Vector3 endPosition;
		Vector3 scale;
		float attackEasingTime;
		float chargeEasingTime;
		float velocity;
	};
	using BossState::BossState;
	void Initialize() override;
	void SetDesc() override;
	void Update() override;
	void OnCollision(const CollisionInfo& collisionInfo) override;
private:
	void ChargeUpdate();
	void AttackUpdate();
	JsonData data_;
	float time_;
};

class BossStateBeamRightAttack:
	public BossState {
public:
	struct JsonData {
		Vector3 startPosition;
		Vector3 endPosition;
		Vector3 scale;
		float attackEasingTime;
		float chargeEasingTime;
	};
	using BossState::BossState;
	void Initialize() override;
	void SetDesc() override;
	void Update() override;
	void OnCollision(const CollisionInfo& collisionInfo) override;
private:
	void ChargeUpdate();
	void AttackUpdate();
	float time_;
};

class BossStateBeamLeftAttack :
	public BossState {
public:
	struct JsonData {
		Vector3 startPosition;
		Vector3 endPosition;
		Vector3 scale;
		float attackEasingTime;
		float chargeEasingTime;
	};
	using BossState::BossState;
	void Initialize() override;
	void SetDesc() override;
	void Update() override;
	void OnCollision(const CollisionInfo& collisionInfo) override;
private:
	void ChargeUpdate();
	void AttackUpdate();
	float time_;
};

class BossStateManager {
public:
	enum State {
		kRoot,
		kHook,
		kLowerAttack,
		kInsideAttack,
		kBeamRightAttack,
		kBeamAttack,
	};

	struct JsonData {
		BossStateRoot::JsonData rootData;
		BossStateHook::JsonData attackData;
		BossStateLowerAttack::JsonData lowerAttackData;
		BossStateInsideAttack::JsonData insideAttackData;
		BossStateBeamRightAttack::JsonData beamRightAttackData;
		BossStateBeamLeftAttack::JsonData beamLeftAttackData;
	};
	BossStateManager(Boss& boss) : boss(boss) {}
	void Initialize();
	void Update();
	void OnCollision(const CollisionInfo& collisionInfo);
	void DrawImGui();

	const State GetState()const { return state_; }

	const JsonData& GetData() { return jsonData_; }
	template<class T>
	void ChangeState(const BossStateManager::State& state) {
		state_ = state;
		static_assert(std::is_base_of_v<BossState, T>, "Not inherited.");
		standbyState_ = std::make_unique<T>(*this);
	}

	Boss& boss;
	JsonData jsonData_;

private:
	std::unique_ptr<BossState> activeState_;
	std::unique_ptr<BossState> standbyState_;
	State state_;
};