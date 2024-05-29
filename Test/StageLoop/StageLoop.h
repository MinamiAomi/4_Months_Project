#pragma once

#include <memory>
#include <vector>
#include <map>

#include "Boss/BossAttackTriggerManager/BossAttackTriggerManager.h"
#include "StageGimmick/Block/BlockManager.h"
#include "StageGimmick/Floor/FloorManager.h"
#include "StageGimmick/FireBar/FireBarManager.h"
#include "StageGimmick/Pendulum/PendulumManager.h"
#include "StageGimmick/StageObject/StageObjectManager.h"
#include "StageGimmick/RevengeCoin/RevengeCoinManager.h"
#include "StageGimmick/BeltConveyor/BeltConveyorManager.h"
#include "StageGimmick/Dropper/DropGimmickManager.h"
#include "Trap/TrapManager.h"
#include "Math/Random.h"

class Boss;
class Camera;
class Player;
class StageLoop {
public:
	void Initialize();
	void Update();
	void Reset();

	void SetCamera(const Camera* camera) { camera_ = camera; }
	void SetBoss(const Boss* boss) { boss_ = boss; }
	void SetPlayer(Player* player) { player_ = player; }

	const std::unique_ptr<BeltConveyorManager>& GetBeltConveyorManager()const { return beltConveyorManager_; }
	const std::unique_ptr<BlockManager>& GetBlockManager()const { return blockManager_; }
	const std::unique_ptr<BossAttackTriggerManager>& GetBossAttackTriggerManager()const { return bossAttackTriggerManager_; }
	const std::unique_ptr<FireBarManager>& GetFireBarManager()const { return fireBarManager_; }
	const std::unique_ptr<FloorManager>& GetFloorManager()const { return floorManager_; }
	const std::unique_ptr<DropGimmickManager>& GetDropGimmickManager()const { return dropGimmickManager_; }
	const std::unique_ptr<PendulumManager>& GetPendulumManager()const { return pendulumManager_; }
	const std::unique_ptr<RevengeCoinManager>& GetRevengeCoinManager()const { return revengeCoinManager_; }
	const std::unique_ptr<StageObjectManager>& GetStageObjectManager()const { return stageObjectManager_; }
	//const std::unique_ptr<TrapManager>& GetTrapManager()const { return trapManager_; }
private:
	const Boss* boss_;
	const Camera* camera_;
	Player* player_;
	static const uint32_t kCreateStageNum = 5;

	struct Desc {
		std::vector<BeltConveyor::Desc>beltConveyorDesc;
		std::vector<Block::Desc>blockDesc;
		std::vector<BossAttackTrigger::Desc>bossAttackTrigger;
		std::vector<FireBar::Desc>fireBarDesc;
		std::vector<Floor::Desc>floorDesc;
		std::vector<DropGimmick::Desc>dropGimmickDesc;
		std::vector<DropperBall::Desc>dropGimmickBallDesc;
		std::vector<Pendulum::Desc>pendulumDesc;
		std::vector<RevengeCoin::Desc>revengeCoinDesc;
		std::vector<StageObject::Desc>stageObjectDesc;
		// ステージ全体の長さ
		float stageSize;
	};

	void LoadJson();

	void InitializeCreateStage(uint32_t stageIndex = (uint32_t)-1);
	void Clear();
	void DeleteObject();
	void CreateStage(uint32_t stageIndex = (uint32_t)-1);
	void CreateStageObject(const Desc& stageData, float distance, uint32_t index);

	struct StageDistance {
		float distance;
		uint32_t stageIndex;
		uint32_t stageNum;
	};

	std::vector<Desc> stageData_;
	std::vector<StageDistance> stageDistance_;
	Random::RandomNumberGenerator rnd_;
	bool isCreateStage_;
	uint32_t stageNum_;
#pragma region
	std::unique_ptr<BeltConveyorManager> beltConveyorManager_;
	std::unique_ptr<BlockManager> blockManager_;
	std::unique_ptr<BossAttackTriggerManager> bossAttackTriggerManager_;
	std::unique_ptr<FireBarManager> fireBarManager_;
	std::unique_ptr<FloorManager> floorManager_;
	std::unique_ptr<DropGimmickManager> dropGimmickManager_;
	std::unique_ptr<PendulumManager> pendulumManager_;
	std::unique_ptr<RevengeCoinManager> revengeCoinManager_;
	std::unique_ptr<StageObjectManager> stageObjectManager_;
	//std::unique_ptr<TrapManager> trapManager_;
#pragma endregion
};