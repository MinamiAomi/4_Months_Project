#include "BossStateManager.h"

#include "CharacterState.h"
#include "Boss/Boss.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"
#include "GameSpeed.h"

void BossStateManager::Initialize() {
	JSON_OPEN("Resources/Data/Boss/Boss.json");
	JSON_OBJECT("StateRoot");
	JSON_LOAD(jsonData_.rootData.velocity);
	JSON_ROOT();
	JSON_OBJECT("StateHook");
	JSON_LOAD(jsonData_.attackData.allFrame);
	JSON_ROOT();
	JSON_OBJECT("StateLowerAttack");
	JSON_LOAD(jsonData_.lowerAttackData.startPosition);
	JSON_LOAD(jsonData_.lowerAttackData.endPosition);
	JSON_LOAD(jsonData_.lowerAttackData.scale);
	JSON_LOAD(jsonData_.lowerAttackData.chargeEasingTime);
	JSON_LOAD(jsonData_.lowerAttackData.attackEasingTime);
	JSON_ROOT();
	JSON_OBJECT("StateInsideAttack");
	JSON_LOAD(jsonData_.insideAttackData.startPosition);
	JSON_LOAD(jsonData_.insideAttackData.endPosition);
	JSON_LOAD(jsonData_.insideAttackData.scale);
	JSON_LOAD(jsonData_.insideAttackData.chargeEasingTime);
	JSON_LOAD(jsonData_.insideAttackData.attackEasingTime);
	JSON_ROOT();
	JSON_CLOSE();
	jsonData_.lowerAttackData.velocity = jsonData_.rootData.velocity;
	jsonData_.insideAttackData.velocity = jsonData_.rootData.velocity;
	state_ = State::kRoot;
}

void BossStateManager::Update() {

	DrawImGui();

	if (standbyState_) {
		activeState_ = std::move(standbyState_);
		activeState_->Initialize();
	}

	if (activeState_) {
		activeState_->Update();
	}
}

void BossStateManager::OnCollision(const CollisionInfo& collisionInfo) {
	if (activeState_) {
		activeState_->OnCollision(collisionInfo);
	}
}

void BossStateManager::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Boss")) {
		const char* items[] = { "Root", "Hook" ,"LowerAttack","InsideAttack" };
		static int selectedItem = static_cast<int>(state_);
		if (ImGui::Combo("State", &selectedItem, items, IM_ARRAYSIZE(items))) {
			state_ = static_cast<State>(selectedItem);
			switch (state_) {
			case State::kRoot:
			{
				ChangeState<BossStateRoot>(State::kRoot);
			}
			break;
			case State::kHook:
			{
				ChangeState<BossStateHook>(State::kHook);
			}
			break;
			case State::kLowerAttack:
			{
				ChangeState<BossStateLowerAttack>(State::kLowerAttack);
			}
			break;
			case State::kInsideAttack:
			{
				ChangeState<BossStateInsideAttack>(State::kInsideAttack);
			}
			break;
			}
		}
		if (ImGui::TreeNode("Root")) {
			ImGui::DragFloat("velocity_", &jsonData_.rootData.velocity, 0.1f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Hook")) {
			ImGui::DragFloat("全体フレーム", &jsonData_.attackData.allFrame, 0.1f, 0.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("LowerAttack")) {
			ImGui::DragFloat3("startPosition", &jsonData_.lowerAttackData.startPosition.x, 0.1f);
			ImGui::DragFloat3("endPosition", &jsonData_.lowerAttackData.endPosition.x, 0.1f);
			ImGui::DragFloat3("scale", &jsonData_.lowerAttackData.scale.x, 0.1f);
			ImGui::DragFloat("chargeEasingTime", &jsonData_.lowerAttackData.chargeEasingTime, 0.1f);
			ImGui::DragFloat("attackEasingTime", &jsonData_.lowerAttackData.attackEasingTime, 0.1f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("InsideAttack")) {
			ImGui::DragFloat3("startPosition", &jsonData_.insideAttackData.startPosition.x, 0.1f);
			ImGui::DragFloat3("endPosition", &jsonData_.insideAttackData.endPosition.x, 0.1f);
			ImGui::DragFloat3("scale", &jsonData_.insideAttackData.scale.x, 0.1f);
			ImGui::DragFloat("chargeEasingTime", &jsonData_.insideAttackData.chargeEasingTime, 0.1f);
			ImGui::DragFloat("attackEasingTime", &jsonData_.insideAttackData.attackEasingTime, 0.1f);
			ImGui::TreePop();
		}
		activeState_->SetDesc();
		if (ImGui::Button("Save")) {
			JSON_OPEN("Resources/Data/Boss/Boss.json");
			JSON_OBJECT("StateRoot");
			JSON_SAVE(jsonData_.rootData.velocity);
			JSON_ROOT();
			JSON_OBJECT("StateHook");
			JSON_SAVE(jsonData_.attackData.allFrame);
			JSON_ROOT();
			JSON_OBJECT("StateLowerAttack");
			JSON_SAVE(jsonData_.lowerAttackData.startPosition);
			JSON_SAVE(jsonData_.lowerAttackData.endPosition);
			JSON_SAVE(jsonData_.lowerAttackData.scale);
			JSON_SAVE(jsonData_.lowerAttackData.chargeEasingTime);
			JSON_SAVE(jsonData_.lowerAttackData.attackEasingTime);
			JSON_ROOT();
			JSON_OBJECT("StateInsideAttack");
			JSON_SAVE(jsonData_.insideAttackData.startPosition);
			JSON_SAVE(jsonData_.insideAttackData.endPosition);
			JSON_SAVE(jsonData_.insideAttackData.scale);
			JSON_SAVE(jsonData_.insideAttackData.chargeEasingTime);
			JSON_SAVE(jsonData_.insideAttackData.attackEasingTime);
			JSON_ROOT();
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
}

void BossStateRoot::Initialize() {
	SetDesc();

	manager_.boss.GetModel()->GetModel(BossParts::Parts::kFloorAll)->SetIsAlive(false);
	manager_.boss.GetModel()->GetModel(BossParts::Parts::kLongDistanceAttack)->SetIsAlive(false);
}

void BossStateRoot::SetDesc() {
	velocity_ = manager_.jsonData_.rootData.velocity;
}

void BossStateRoot::Update() {}

void BossStateRoot::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void BossStateHook::Initialize() {
	SetDesc();
	manager_.boss.GetModel()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(0).SetIsCollision(true);
	time_ = 0.0f;
}

void BossStateHook::SetDesc() {
	data_ = manager_.jsonData_.attackData;
}

void BossStateHook::Update() {
	float t = time_ / data_.allFrame;
	auto& parts = manager_.boss.GetModel()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(0);
	parts.skeleton->ApplyAnimation(parts.animation->GetAnimation("armAttack"), t);
	parts.skeleton->Update();

	parts.UpdateCollider(manager_.boss.GetModel()->GetModel(BossParts::Parts::kBossBody)->transform.worldMatrix);

	manager_.boss.GetModel()->GetModel(BossParts::Parts::kBossBody)->GetModel()->SetSkeleton(parts.skeleton);
	time_ += 1.0f;
	if (t >= 1.0f) {
		manager_.boss.GetModel()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(0).SetIsCollision(false);
	manager_.boss.GetModel()->GetModel(BossParts::Parts::kBossBody)->GetModel()->SetSkeleton(nullptr);
		manager_.ChangeState<BossStateRoot>(BossStateManager::State::kRoot);
	}
}

void BossStateHook::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void BossStateLowerAttack::Initialize() {
	SetDesc();
	attackState_ = kChage;
	time_ = 0.0f;
}

void BossStateLowerAttack::SetDesc() {
	data_ = manager_.jsonData_.lowerAttackData;
	manager_.boss.GetModel()->GetModel(BossParts::Parts::kFloorAll)->transform.scale = data_.scale;
	manager_.boss.GetModel()->GetModel(BossParts::Parts::kFloorAll)->SetModelIsAlive(true);
}

void BossStateLowerAttack::Update() {
	switch (attackState_) {
	case BossState::kChage:
	{
		ChargeUpdate();
	}
	break;
	case BossState::kAttack:
	{
		AttackUpdate();
	}
	break;
	default:
		break;
	}
}

void BossStateLowerAttack::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void BossStateLowerAttack::ChargeUpdate() {
	auto& floorAllTransform = manager_.boss.GetModel()->GetModel(BossParts::Parts::kFloorAll)->transform;
	float t = time_ / data_.chargeEasingTime;
	time_ += 1.0f;
	floorAllTransform.translate.x = std::lerp(data_.startPosition.x, data_.endPosition.x, t);
	floorAllTransform.translate.y = std::lerp(data_.startPosition.y, data_.endPosition.y, t);
	floorAllTransform.translate.z = std::lerp(data_.startPosition.z, data_.endPosition.z, t);

	if (t >= 1.0f) {
		attackState_ = kAttack;
		time_ = 0.0f;
		manager_.boss.GetModel()->GetModel(BossParts::Parts::kFloorAll)->SetColliderIsAlive(true);
	}
}

void BossStateLowerAttack::AttackUpdate() {
	float t = time_ / data_.chargeEasingTime;
	time_ += 1.0f;
	manager_.boss.GetModel()->GetModel(BossParts::Parts::kFloorAll)->GetModel()->SetColor({ 1.0f,0.0f,0.0f });
	if (t >= 1.0f) {
		manager_.boss.GetModel()->GetModel(BossParts::Parts::kFloorAll)->SetIsAlive(false);
		manager_.boss.GetModel()->GetModel(BossParts::Parts::kFloorAll)->GetModel()->SetColor({ 1.0f,1.0f,1.0f });
		manager_.ChangeState<BossStateRoot>(BossStateManager::State::kRoot);
	}
}

void BossStateInsideAttack::Initialize() {
	SetDesc();
	attackState_ = kChage;
	time_ = 0.0f;
}

void BossStateInsideAttack::SetDesc() {
	data_ = manager_.jsonData_.insideAttackData;
	manager_.boss.GetModel()->GetModel(BossParts::Parts::kLongDistanceAttack)->transform.scale = data_.scale;
	manager_.boss.GetModel()->GetModel(BossParts::Parts::kLongDistanceAttack)->SetModelIsAlive(true);
}

void BossStateInsideAttack::Update() {
	switch (attackState_) {
	case BossState::kChage:
	{
		ChargeUpdate();
	}
	break;
	case BossState::kAttack:
	{
		AttackUpdate();
	}
	break;
	default:
		break;
	}
}

void BossStateInsideAttack::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void BossStateInsideAttack::ChargeUpdate() {
	auto& longDistanceAttackTransform = manager_.boss.GetModel()->GetModel(BossParts::Parts::kLongDistanceAttack)->transform;
	float t = time_ / data_.chargeEasingTime;
	time_ += 1.0f;
	longDistanceAttackTransform.translate.x = std::lerp(data_.startPosition.x, data_.endPosition.x, t);
	longDistanceAttackTransform.translate.y = std::lerp(data_.startPosition.y, data_.endPosition.y, t);
	longDistanceAttackTransform.translate.z = std::lerp(data_.startPosition.z, data_.endPosition.z, t);

	if (t >= 1.0f) {
		attackState_ = kAttack;
		time_ = 0.0f;
		manager_.boss.GetModel()->GetModel(BossParts::Parts::kLongDistanceAttack)->SetColliderIsAlive(true);
	}
}

void BossStateInsideAttack::AttackUpdate() {
	float t = time_ / data_.chargeEasingTime;
	time_ += 1.0f;
	manager_.boss.GetModel()->GetModel(BossParts::Parts::kLongDistanceAttack)->GetModel()->SetColor({ 1.0f,0.0f,0.0f });
	if (t >= 1.0f) {
		manager_.boss.GetModel()->GetModel(BossParts::Parts::kLongDistanceAttack)->SetIsAlive(false);
		manager_.boss.GetModel()->GetModel(BossParts::Parts::kLongDistanceAttack)->GetModel()->SetColor({ 1.0f,1.0f,1.0f });
		manager_.ChangeState<BossStateRoot>(BossStateManager::State::kRoot);
	}
}

void BossStateBeamRightAttack::Initialize() {
	SetDesc();
	attackState_ = kChage;
	time_ = 0.0f;
}

void BossStateBeamRightAttack::SetDesc() {

}

void BossStateBeamRightAttack::Update() {}

void BossStateBeamRightAttack::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void BossStateBeamRightAttack::ChargeUpdate() {}

void BossStateBeamRightAttack::AttackUpdate() {}

void BossStateBeamLeftAttack::Initialize() {}

void BossStateBeamLeftAttack::SetDesc() {}

void BossStateBeamLeftAttack::Update() {}

void BossStateBeamLeftAttack::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void BossStateBeamLeftAttack::ChargeUpdate() {}

void BossStateBeamLeftAttack::AttackUpdate() {}
