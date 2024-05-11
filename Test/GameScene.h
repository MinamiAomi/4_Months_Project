#pragma once
#include "Scene/BaseScene.h"

#include <memory>
#include <vector>

#include "Audio/AudioSource.h"
#include "Graphics/Model.h"
#include "Graphics/LightManager.h"
#include "Math/Camera.h"
#include "Math/Transform.h"
#include "Math/Random.h"

#include "StageLoop/StageLoop.h"
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
#include "UI.h"
#include "CutIn.h"
#include "Trap/TrapManager.h"

class GameScene :
	public BaseScene {
public:

	void OnInitialize() override;
	void OnUpdate() override;
	void OnFinalize() override;
	void Initialize();

private:
	std::shared_ptr<CameraManager> cameraManager_;
	std::shared_ptr<DirectionalLight> directionalLight_;

#pragma region 
	std::unique_ptr<EditorManager> editorManager_;
	bool isMove_;
#pragma endregion

	std::unique_ptr<StageLoop> stageLoop_;

#pragma region ゲーム
	std::unique_ptr<Player> player_;
	std::unique_ptr<PlayerDustParticle> playerDustParticle_;
	std::unique_ptr<Boss> boss_;
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<StageLineLight> stageRightLight;
	std::unique_ptr<StageLineLight> stageLeftLight;

	std::unique_ptr<StageLineLight> stageUpRightLight;
	std::unique_ptr<StageLineLight> stageUpLeftLight;

	std::unique_ptr<StageBlockManager> stageBlockManager_;
	std::unique_ptr<SkyBlockManager> skyBlockManager_;

	std::unique_ptr<UI> ui_;
	std::unique_ptr<CutIn> cutIn_;
#pragma endregion

	AudioSource bgm_;
	std::shared_ptr<Sound> chaseBGM_;
	std::shared_ptr<Sound> revengeBGM_;
};
