#include "Boss.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"

void Boss::Initialize() {
#pragma endregion
	JSON_OPEN("Resources/Data/Boss/Boss.json");
	JSON_OBJECT("Boss");
	JSON_LOAD(offset_);
	JSON_ROOT();
	JSON_CLOSE();
	bossModelManager_ = std::make_unique<BossModelManager>();
	bossModelManager_->Initialize(&transform);
	// 隠す
	bossModelManager_->GetModel(BossParts::kFloorAll)->SetIsAlive(false);
	bossModelManager_->GetModel(BossParts::kLongDistanceAttack)->SetIsAlive(false);
	isMove_ = true;

	state_ = std::make_unique<BossStateManager>(*this);
	state_->Initialize();
	state_->ChangeState<BossStateRoot>();

	bossAttackTriggerManager_ = std::make_unique<BossAttackTriggerManager>();
	bossAttackTriggerManager_->SetBoss(this);
	bossAttackTriggerManager_->Initialize();

	Reset();
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Boss");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	// 鉾方向にくっそでかく（プレイヤーの弾がうしろにいかないよう）
	collider_->SetSize({ transform.scale.x*10.0f,transform.scale.y,transform.scale.z});
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
			JSON_LOAD(offset_);
			JSON_ROOT();
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
	/*time_ -= 1.0f;
	if (time_ <= 0.0f) {
		state_->ChangeState<BossStateAttack>();
		time_ = interval_;
	}*/
	state_->Update();
	UpdateTransform();
	bossModelManager_->Update();
	bossAttackTriggerManager_->Update();
}

void Boss::Reset() {
	transform.translate = offset_;
	transform.rotate = Quaternion::identity;
	transform.scale = { 7.0f,7.0f,7.0f };
	state_->ChangeState<BossStateRoot>();
	time_ = interval_;
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

	}
}
