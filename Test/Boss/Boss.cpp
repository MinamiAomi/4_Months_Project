#include "Boss.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"
#include "GameSpeed.h"
#include "Player/Player.h"

void Boss::Initialize() {
#pragma endregion
	JSON_OPEN("Resources/Data/Boss/Boss.json");
	JSON_OBJECT("Boss");
	JSON_LOAD(offset_);
	JSON_ROOT();
	JSON_CLOSE();

	state_ = std::make_unique<BossStateManager>(*this);
	state_->Initialize();
	state_->ChangeState<BossStateRoot>(BossStateManager::State::kRoot);

	bossAttackTriggerManager_ = std::make_unique<BossAttackTriggerManager>();
	bossAttackTriggerManager_->SetCamera(camera_);
	bossAttackTriggerManager_->SetBoss(this);
	bossAttackTriggerManager_->Initialize(0);

	Reset(0);

	bossModelManager_ = std::make_unique<BossModelManager>();
	bossModelManager_->Initialize(&transform);

	// 隠す
	bossModelManager_->GetModel(BossParts::kFloorAll)->SetIsAlive(false);
	bossModelManager_->GetModel(BossParts::kLongDistanceAttack)->SetIsAlive(false);
	isMove_ = true;

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Boss");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	// 鉾方向にくっそでかく（プレイヤーの弾がうしろにいかないよう
	// ）
	Vector3 modelSize = (bossModelManager_->GetModel(BossParts::kBody)->GetModel()->GetModel()->GetMeshes().at(0).maxVertex - bossModelManager_->GetModel(BossParts::kBody)->GetModel()->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * 2.0f,modelSize.y ,modelSize.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Boss);
	collider_->SetCollisionMask(~CollisionAttribute::Boss);
	collider_->SetIsActive(true);
#pragma endregion
}

void Boss::Update() {
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Boss")) {
		ImGui::DragFloat3("Pos", &transform.translate.x);
		ImGui::DragFloat3("offset_", &offset_.x);
		if (ImGui::Button("OffsetSave")) {
			JSON_OPEN("Resources/Data/Boss/Boss.json");
			JSON_OBJECT("Boss");
			JSON_SAVE(offset_);
			JSON_ROOT();
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
	if (Character::IsSceneChange()) {
		easingStartPosition_ = transform.translate;
	}

	switch (Character::currentCharacterState_) {
	case Character::State::kChase:
	{
		if (isMove_) {
			transform.translate.z += GameSpeed::GetGameSpeed();
		}
	}
	break;
	case Character::State::kRunAway:
	{
		if (isMove_) {
			transform.translate.z -= GameSpeed::GetGameSpeed();
		}
	}
	break;
	case Character::State::kScneChange:
	{
		if (Character::nextCharacterState_ == Character::State::kChase) {
			transform.translate.z = std::lerp(easingStartPosition_.z, player_->transform.worldMatrix.GetTranslate().z + player_->GetChaseLimitLine(), Character::GetSceneChangeTime());
		}
	}
	break;
	default:
		break;
	}

	state_->Update();
	UpdateTransform();
	bossModelManager_->Update();
	bossAttackTriggerManager_->Update();
}

void Boss::Reset(uint32_t stageIndex) {
	isAlive_ = true;
	transform.translate = offset_;
	transform.rotate = Quaternion::identity;
	transform.scale = Vector3::one;
	transform.UpdateMatrix();
	state_->ChangeState<BossStateRoot>(BossStateManager::State::kRoot);
	bossAttackTriggerManager_->Reset(stageIndex);
}

void Boss::UpdateTransform() {
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	collider_->SetOrientation(rotate);
}

void Boss::OnCollision(const CollisionInfo& collisionInfo) {
	state_->OnCollision(collisionInfo);
	if (collisionInfo.collider->GetName() == "Player") {
		switch (Character::currentCharacterState_) {
		case Character::State::kChase:
		{
			isAlive_ = false;
		}
		break;
		case Character::State::kRunAway:
		{

		}
		break;
		default:
			break;
		}
	}
}
