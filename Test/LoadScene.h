#pragma once
#include "Scene/BaseScene.h"

#include <memory>

#include "Graphics/Sprite.h"

class Test;

class LoadScene :
	public BaseScene {
public:
    enum Member {
        Rikihisa,
        Aomi,
        Mint,
        Fuga,
        Sakiren,

        NumMembers
    };

	void OnInitialize() override;
	void OnUpdate() override;
	void OnFinalize() override;

	void SetTest(Test* test) { test_ = test; }

private:
	std::unique_ptr<Sprite> members_[NumMembers];
	std::unique_ptr<Sprite> donburi_;
	std::unique_ptr<Sprite> logo_;

	float animationTime_;
	Test* test_;
};
