#pragma once

#include "Editor/BlockEditor/BlockEditor.h"
#include "Editor/FireBarEditor/FireBarEditor.h"
#include "Editor/FloorEditor/FloorEditor.h"
#include "Editor/PendulumEditor/PendulumEditor.h"
#include "Editor/BossAttackTriggerEditor/BossAttackTriggerEditor.h"

#include "Player/Player.h"

class BlockManager;
class FireBarManager;
class FloorManager;
class PendulumManager;
class BossAttackTriggerManager;
class Player;
class Boss;
class EditorManager {
public:
	void Initialize(BlockManager* blockEditor, FireBarManager* fireBarEditor, FloorManager* floorEditor, PendulumManager* pendulumManager, BossAttackTriggerManager* bossAttackTriggerManager);

	void Update();

	void SetPlayer(const Player* player) { player_ = player; }
	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetCamera(const Camera* camera) { camera_= camera; }
private:
	const Camera* camera_;
	const Player* player_;
	const Boss* boss_;

	int stageIndex_;

	std::unique_ptr<BlockEditor> blockEditor_;
	std::unique_ptr<FireBarEditor> fireBarEditor_;
	std::unique_ptr<FloorEditor> floorEditor_;
	std::unique_ptr<PendulumEditor> pendulumEditor_;
	std::unique_ptr<BossAttackTriggerEditor> bossAttackTriggerEditor_;
};