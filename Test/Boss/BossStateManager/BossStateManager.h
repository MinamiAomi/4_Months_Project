#pragma once
#include "Collision/Collider.h"

#include <memory>
#include <array>

#include "Math/MathUtils.h"
#include "Collision/Collider.h"

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
		Vector3 startPosition;
		Vector3 endPosition;
		Vector3 startRotate;
		Vector3 endRotate;
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
	// このstateに入った時の位置->アタックが始まる位置
	Vector3 initialPosition_;
	Vector3 initialRotate_;
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

class BossStateRainOfArrow :
	public BossState {
public:
	struct JsonData {
		Vector3 startPosition;
		Vector3 endPosition;
		Vector3 startRotate;
		Vector3 endRotate;
		float easingTime;
		float velocity;
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

class BossStateArmHammer :
	public BossState {
public:
	struct JsonData {
		Vector3 startPosition;
		Vector3 endPosition;
		Vector3 startRotate;
		Vector3 endRotate;
		float easingTime;
		float velocity;
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



class BossStateManager {
public:
	enum State {
		kRoot,
		kHook,
		kLowerAttack,
		kInsideAttack,
		kRainOfArrow,
		kArmHammer,

	};

	struct JsonData {
		BossStateRoot::JsonData rootData;
		BossStateHook::JsonData attackData;
		BossStateLowerAttack::JsonData lowerAttackData;
		BossStateInsideAttack::JsonData insideAttackData;
	};
	BossStateManager(Boss& boss) : boss(boss) {}
	void Initialize();
	void Update();
	void OnCollision(const CollisionInfo& collisionInfo);
	void DrawImGui();

	const State GetState()const { return state_; }

	const JsonData& GetData() { return jsonData_; }
	template<class T>
	void ChangeState() {
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