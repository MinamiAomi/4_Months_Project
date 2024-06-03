#pragma once
#include "Scene/BaseScene.h"

#include <array>
#include <memory>
#include <vector>

#include "Graphics/LightManager.h"
#include "Math/Camera.h"
#include "Graphics/Sprite.h"
#include "Graphics/Model.h"
#include "TitleUI.h"

#include "DebugCamera.h"
#include "Player/TitlePlayer/TitlePlayer.h"
#include "TitleFloor.h"
#include "Boss/TitleBoss/TitleBoss.h"

class TitleScene :
	public BaseScene {
public:

	void OnInitialize() override;
	void OnUpdate() override;
	void OnFinalize() override;
private:
	std::shared_ptr<Camera> camera_;
	std::unique_ptr<DebugCamera> debugCamera_;
	std::shared_ptr<DirectionalLight> directionalLight_;

	std::unique_ptr<Sprite> title_;
	std::unique_ptr<TitleUI> UI_;
	std::unique_ptr<TitlePlayer> titlePlayer_;
	std::array<std::unique_ptr<TitleFloor>,3>titleFloor_;
	std::unique_ptr<TitleBoss> titleBoss_;

	bool isDebugCamera_;
};
