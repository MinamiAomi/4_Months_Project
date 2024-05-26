#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "StageGimmick/Pendulum/PendulumManager.h"
#include "StageGimmick/Pendulum/Pendulum.h"
#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "Player/Player.h"

class PendulumEditor {
public:
	void Initialize();
	void Update();

	void SetPendulumManager(PendulumManager* pendulumManager) { pendulumManager_ = pendulumManager; }
	void SaveFile(uint32_t stageName);
	void Clear();


	void SetPlayer(const Player* player) { player_ = player; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	static const std::string kModelName;

	void OnCollision(const CollisionInfo& collisionInfo);

	const Player* player_;
	const Camera* camera_;

	PendulumManager* pendulumManager_;

	std::unique_ptr<Pendulum> pendulum_;

	std::string fileName_;

	int stageIndex_;

	bool isCreate_;
};