#pragma once
#include "Scene/BaseScene.h"

#include <memory>
#include <vector>

#include "Graphics/LightManager.h"
#include "Math/Camera.h"
#include "Graphics/Sprite.h"
#include "Graphics/Model.h"
#include "TitleUI.h"

class TitleScene :
	public BaseScene {
public:

	void OnInitialize() override;
	void OnUpdate() override;
	void OnFinalize() override;
private:
	std::shared_ptr<Camera> camera_;
	std::shared_ptr<DirectionalLight> directionalLight_;

	std::unique_ptr<Sprite> title_;
	std::unique_ptr<TitleUI> UI_;
};
