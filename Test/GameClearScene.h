#pragma once
#include "Scene/BaseScene.h"

#include <array>
#include <memory>
#include <vector>

#include "DebugCamera.h"
#include "Graphics/LightManager.h"
#include "Math/Camera.h"
#include "Graphics/Sprite.h"
#include "Boss/GameClearBoss/GameClearBoss.h"
#include "Player/GameClearPlayer/GameClearPlayer.h"
#include "TitleFloor.h"

class GameClearScene :
	public BaseScene {
public:

	void OnInitialize() override;
	void OnUpdate() override;
	void OnFinalize() override;
private:
	std::unique_ptr<DebugCamera> debugCamera_;
	std::shared_ptr<Camera> camera_;
	std::shared_ptr<DirectionalLight> directionalLight_;

	std::unique_ptr<Sprite> title_;
	std::unique_ptr<GameClearBoss> gameClearBoss_;
	std::unique_ptr<GameClearPlayer> gameClearPlayer_;
	std::array<std::unique_ptr<TitleFloor>, 3>titleFloor_;
	bool isDebugCamera_;
};
