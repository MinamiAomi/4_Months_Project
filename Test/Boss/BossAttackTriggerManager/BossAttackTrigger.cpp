#include "BossAttackTrigger.h"

#include "Boss/Boss.h"
#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ImGuiManager.h"
#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"

void BossAttackTrigger::Initialize(const Desc& desc) {
	Reset();
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("box"));
	model_->SetIsActive(false);

	desc_ = desc;

	transform.translate.z = desc_.desc.transform.translate.z;
	transform.scale = desc_.desc.transform.scale;

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("BossAttackTrigger");
	collider_->SetCenter(desc_.desc.collider->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * desc_.desc.collider->rotate);
	collider_->SetSize({ transform.scale.x * desc_.desc.collider->size.x ,transform.scale.y * desc_.desc.collider->size.y ,transform.scale.z * desc_.desc.collider->size.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::BossAttackTrigger);
	collider_->SetCollisionMask(CollisionAttribute::Boss);
	collider_->SetIsActive(true);
#pragma endregion
}

void BossAttackTrigger::Update() {
	if (std::fabs((camera_->GetPosition() - transform.worldMatrix.GetTranslate()).Length()) <= 200.0f) {
		//model_->SetIsActive(true);
		collider_->SetIsActive(true);
	}
	else {
		//model_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
	transform.UpdateMatrix();
	collider_->SetCenter(desc_.desc.collider->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * desc_.desc.collider->rotate);
	collider_->SetSize({ transform.scale.x * desc_.desc.collider->size.x ,transform.scale.y * desc_.desc.collider->size.y ,transform.scale.z * desc_.desc.collider->size.z });
	model_->SetWorldMatrix(transform.worldMatrix);
}

void BossAttackTrigger::Reset() {
	isCollision_ = false;
}

void BossAttackTrigger::SetDesc(const Desc& desc) {
	desc_ = desc;

	transform.translate.z = desc_.desc.transform.translate.z;
	transform.UpdateMatrix();
}

void BossAttackTrigger::SetIsAlive(bool flag) {
	collider_->SetIsActive(flag);
	model_->SetIsActive(flag);
}

void BossAttackTrigger::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Boss") {

		if (boss_->GetStateManager()->GetState() == BossStateManager::State::kRoot &&
			!isCollision_) {
			if (Character::currentCharacterState_ == Character::kRunAway) {
				switch (desc_.state) {
				case BossStateManager::kRoot:
					boss_->GetStateManager()->ChangeState(BossStateManager::State::kRoot);
					break;
				case BossStateManager::kHook:
					boss_->GetStateManager()->ChangeState(BossStateManager::State::kHook);
					break;
				case BossStateManager::kLowerAttack:
					boss_->GetStateManager()->ChangeState(BossStateManager::State::kLowerAttack);
					break;
				case BossStateManager::kInsideAttack:
					boss_->GetStateManager()->ChangeState(BossStateManager::State::kInsideAttack);
					break;
				default:
					break;
				}

			}
			else {
				switch (desc_.state) {

				case BossStateManager::kBeamAttack:
				{
					boss_->GetStateManager()->ChangeState(BossStateManager::State::kBeamAttack);

					// BossModelManager を取得
					BossModelManager* bossModelManager = boss_->GetModelManager().get();
					if (!bossModelManager) {
						// エラーハンドリング: bossModelManager が null の場合
						assert(0);
					}

					// kBeamAttack モデルを取得
					auto& model = bossModelManager->GetModel(BossParts::kBeamAttack);

					// BeamAttack 型にキャスト
					BeamAttack* beamAttackModel = dynamic_cast<BeamAttack*>(model.get());
					if (beamAttackModel) {
						// vector_ にアクセス
						Vector3 vector = beamAttackModel->vector_ = desc_.vector;
						// 必要な操作を行う

					}
				}
				break;
				default:
					break;
				}
			}
			isCollision_ = true;

		}
	}

}