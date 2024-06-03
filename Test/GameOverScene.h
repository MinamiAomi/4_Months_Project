#pragma once
#include "Scene/BaseScene.h"

#include <memory>
#include <vector>

#include "DebugCamera.h"
#include "Graphics/LightManager.h"
#include "Math/Camera.h"
#include "Graphics/Sprite.h"
#include "Boss/GameOverBoss/GameOverBoss.h"

class GameOverScene :
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
	std::unique_ptr<GameOverBoss> gameOverBoss_;
	bool isDebugCamera_;
};
