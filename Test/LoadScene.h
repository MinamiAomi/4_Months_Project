#pragma once
#include "Scene/BaseScene.h"

#include <memory>

#include "Graphics/Sprite.h"

class Test;

class LoadScene :
	public BaseScene {
public:

	void OnInitialize() override;
	void OnUpdate() override;
	void OnFinalize() override;

	void SetTest(Test* test) { test_ = test; }

private:
	std::unique_ptr<Sprite> load_;
	float animationTime_;
	Test* test_;
};
