#include "StageBlockManager.h"

#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"

#include "StageBlock.h"

void StageBlockManager::Initialize() {
	blockAppear_ = kAppearTime_;
}

void StageBlockManager::Update() {
	blockAppear_--;
	if (blockAppear_ <= 0) {
		blockAppear_ = kAppearTime_;
		Appear();
	}

	for (const std::unique_ptr<StageBlock>& stageBlock : stageBlocks_) {
		stageBlock->Update();
	}
	DeadCheck();
	UpdateTransform();
}

void StageBlockManager::Appear()
{
	if (boss_) {
		std::unique_ptr<StageBlock> leftStageBlock = std::make_unique<StageBlock>();
		std::unique_ptr<StageBlock> rightStageBlock = std::make_unique<StageBlock>();
		leftStageBlock->Initialize({ -32.0f,-30.0f, boss_->transform.worldMatrix.GetTranslate().z  - 30.0f}, {0.0f,1.0f,0.0f});
		rightStageBlock->Initialize({ 32.0f,-30.0f, boss_->transform.worldMatrix.GetTranslate().z - 30.0f }, { 0.0f,1.0f,0.0f });
		stageBlocks_.push_back(std::move(leftStageBlock));
		stageBlocks_.push_back(std::move(rightStageBlock));
	}
}

void StageBlockManager::DeadCheck()
{
	stageBlocks_.remove_if([](std::unique_ptr<StageBlock>& stageBlock) {
		if (stageBlock->GetIsDead()) {
			return true;
		}
		return false;
	});
}

void StageBlockManager::UpdateTransform() {
	transform.UpdateMatrix();
}

