#pragma once

#include <memory>
#include <array>

#include "Math/MathUtils.h"

class Boss;
class BossState;
struct CollisionInfo;
class BossStateManager;

class BossState {
public:
	BossState(BossStateManager& manager) : manager_(manager) {}
	virtual ~BossState() {}
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void OnCollision(const CollisionInfo& collisionInfo) = 0;
	BossStateManager& GetManager() { return manager_; }
protected:
	BossStateManager& manager_;
};

class BossStateRoot :
	public BossState {
public:
	struct JsonData {
		float velocity;
	};
	using BossState::BossState;
	void Initialize() override;
	void Update() override;
	void OnCollision(const CollisionInfo& collisionInfo) override;

private:
	float velocity_;
};

class BossStateAttack :
	public BossState {
public:
	struct JsonData {
		Vector3 startPosition;
		Vector3 endPosition;
		Vector3 rotate;
		float easingTime;
	};
	using BossState::BossState;
	void Initialize() override;
	void Update() override;
	void OnCollision(const CollisionInfo& collisionInfo) override;

private:
	Vector3 startPosition_;
	Vector3 endPosition_;
	Vector3 rotate_;
	float time_;
	float easingTime_;
};


class BossStateManager {
public:
	enum State {
		kRoot,
		kAttack,
	};

	struct JsonData {
		BossStateRoot::JsonData rootData;
		BossStateAttack::JsonData attackData;
	};
	BossStateManager(Boss& boss) : boss(boss) {}
	void Initialize();
	void Update();
	void OnCollision(const CollisionInfo& collisionInfo);
	void DrawImGui();

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