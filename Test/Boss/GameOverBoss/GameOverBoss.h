#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Engine/Graphics/Model.h"

class GameOverBoss :
	public GameObject {
public:
	void Initialize();

	void Update();

private:
	void UpdateTransform();

	std::unique_ptr<ModelInstance> model_;
	std::shared_ptr<Animation> animation_;
	std::shared_ptr<Skeleton> skeleton_;
	float animationTime_;
	float animationAllFrame_;
	float offset_;
};