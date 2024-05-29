#include "StageBlockManager.h"

#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"
#include "CharacterState.h"

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
}

void StageBlockManager::Reset()
{
	stageBlocks_.clear();
}

void StageBlockManager::Appear()
{

	const float blockPosY = -30.0f;
	const float blockPosZ = 40.0f;

	if (boss_ && isMove_ && (Character::currentCharacterState_ == Character::kChase || Character::currentCharacterState_ == Character::kRunAway)) {
		std::unique_ptr<StageBlock> leftStageBlock = std::make_unique<StageBlock>();
		std::unique_ptr<StageBlock> rightStageBlock = std::make_unique<StageBlock>();
		
		Vector3 direction = Vector3{ rng_.NextFloatRange(-0.2f, 0.2f), rng_.NextFloatRange(0.8f, 1.0f),0.0f }.Normalize();
		leftStageBlock->Initialize({ -32.0f,blockPosY, boss_->transform.worldMatrix.GetTranslate().z  - blockPosZ }, direction, rng_.NextIntRange(16,20));
		rightStageBlock->Initialize({ 32.0f,blockPosY, boss_->transform.worldMatrix.GetTranslate().z - blockPosZ }, direction, rng_.NextIntRange(16, 20));
		leftStageBlock->SetBoss(boss_);
		rightStageBlock->SetBoss(boss_);
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

