#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Engine/Graphics/Model.h"

class GameClearPlayer :
	public GameObject {
public:
	void Initialize();

	void Update();

private:
	void Jump();
	void UpdateTransform();

	std::unique_ptr<ModelInstance> model_;
	std::shared_ptr<Animation> animation_;
	std::shared_ptr<Skeleton> skeleton_;
	float animationTime_;
	float animationAllFrame_;
	float ground_;
	float gravity_;
	float acceleration_;
	float jumpPower_;

	float jumpTime_;
	float maxJumpTime_;
	float maxRotateTime_;
	bool isFirst_;
};