#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "StageGimmick/FireBar/FireBarManager.h"
#include "Player/Player.h"

class FireBarEditor
	: public GameObject {
public:
	void Initialize();
	void Update();

	void SetFireManager(FireBarManager* fireBarManager) { fireBarManager_ = fireBarManager; }
	void SaveFile(uint32_t stageName);
	void LoadFile(uint32_t stageName);
	void Clear();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetCamera(const Camera* camera) { camera_ = camera; }
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);

	const Player* player_;
	const Camera* camera_;

	FireBarManager* fireBarManager_;

	std::string fileName_;

	int stageIndex_;

	bool isCreate_;

	FireBar::Desc desc_;
	std::unique_ptr<FireBar> fireBar_;
};