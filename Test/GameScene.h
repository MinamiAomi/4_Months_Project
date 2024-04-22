#pragma once
#include "Scene/BaseScene.h"

#include <memory>
#include <vector>

#include "Graphics/Model.h"
#include "Graphics/LightManager.h"
#include "Math/Camera.h"
#include "Math/Transform.h"
#include "Math/Random.h"

#include "StageGimmick/Block/BlockManager.h"
#include "StageGimmick/Floor/FloorManager.h"
#include "StageGimmick/FireBar/FireBarManager.h"
#include "StageGimmick/Pendulum/PendulumManager.h"
#include "Boss/Boss.h"
#include "CameraManager/CameraManager.h"
#include "DebugCamera.h"
#include "Editor/EditorManager.h"
#include "Player/Player.h"
#include "Player/PlayerParticle/PlayerDustParticle.h"
#include "Skydome.h"
#include "StageLineLight.h"
#include "StageBlockManager.h"
#include "StageBlock.h"
#include "SkyBlockManager.h"
#include "SkyBlock.h"

class GameScene :
	public BaseScene {
public:

	void OnInitialize() override;
	void OnUpdate() override;
	void OnFinalize() override;

private:
	std::shared_ptr<CameraManager> cameraManager_;
	std::shared_ptr<DirectionalLight> directionalLight_;

#pragma region 
	std::unique_ptr<EditorManager> editorManager_;
	bool isMove_;
#pragma endregion
#pragma region 
	std::unique_ptr<BlockManager> blockManager_;
	std::unique_ptr<FireBarManager> fireBarManager_;
	std::unique_ptr<FloorManager> floorManager_;
	std::unique_ptr<PendulumManager> pendulumManager_;
#pragma endregion

#pragma region ゲーム
	std::unique_ptr<Player> player_;
	std::unique_ptr<PlayerDustParticle> playerDustParticle_;
	std::unique_ptr<Boss> boss_;
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<StageLineLight> stageRightLight;
	std::unique_ptr<StageLineLight> stageLeftLight;
	std::unique_ptr<StageBlockManager> stageBlockManager_;
	std::unique_ptr<SkyBlockManager> skyBlockManager_;
#pragma endregion
};
