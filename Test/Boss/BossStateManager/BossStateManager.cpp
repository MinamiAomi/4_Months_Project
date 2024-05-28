#include "BossStateManager.h"

#include "CharacterState.h"
#include "Boss/Boss.h"
#include "Player/Player.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"
#include "GameSpeed.h"


void BossStateManager::Initialize() {
	JSON_OPEN("Resources/Data/Boss/Boss.json");
	JSON_OBJECT("StateRoot");
	JSON_LOAD(jsonData_.rootData.allFrame);
	JSON_ROOT();
	JSON_OBJECT("StateHook");
	JSON_LOAD(jsonData_.attackData.allFrame);
	JSON_ROOT();
	JSON_OBJECT("StateLowerAttack");
	JSON_LOAD(jsonData_.lowerAttackData.position);
	JSON_LOAD(jsonData_.lowerAttackData.scale);
	JSON_LOAD(jsonData_.lowerAttackData.chargeEasingTime);
	JSON_LOAD(jsonData_.lowerAttackData.attackEasingTime);
	JSON_ROOT();
	JSON_OBJECT("StateInsideAttack");
	JSON_LOAD(jsonData_.insideAttackData.position);
	JSON_LOAD(jsonData_.insideAttackData.scale);
	JSON_LOAD(jsonData_.insideAttackData.chargeEasingTime);
	JSON_LOAD(jsonData_.insideAttackData.attackEasingTime);
	JSON_ROOT();
	JSON_OBJECT("StateBeamAttack");
	JSON_LOAD(jsonData_.beamAttackData.startPosition);
	JSON_LOAD(jsonData_.beamAttackData.endPosition);
	JSON_LOAD(jsonData_.beamAttackData.scale);
	JSON_LOAD(jsonData_.beamAttackData.chargeEasingTime);
	JSON_LOAD(jsonData_.beamAttackData.attackEasingTime);
	JSON_ROOT();
	JSON_CLOSE();
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
		const char* items[] = { "Root", "Hook" ,"InsideAttack","LowerAttack" ,"BeamAttack" };
		static int selectedItem = static_cast<int>(state_);
		if (ImGui::Combo("State", &selectedItem, items, IM_ARRAYSIZE(items))) {
			state_ = static_cast<State>(selectedItem);
			switch (state_) {
			case State::kRoot:
			{
				ChangeState(State::kRoot);
			}
			break;
			case State::kHook:
			{
				ChangeState(State::kHook);
			}
			break;
			case State::kLowerAttack:
			{
				ChangeState(State::kLowerAttack);
			}
			break;
			case State::kInsideAttack:
			{
				ChangeState(State::kInsideAttack);
			}
			break;
			case State::kBeamAttack:
			{
				ChangeState(State::kBeamAttack);
				// BossModelManager を取得
				auto& tmp = boss.GetModelManager()->GetModel(BossParts::kBeamAttack);
				if (!tmp) {
					// エラーハンドリング: bossModelManager が null の場合
					assert(0);
				}
				// BeamAttack 型にキャスト
				BeamAttack* beamAttackModel = dynamic_cast<BeamAttack*>(tmp.get());
				if (beamAttackModel) {
					beamAttackModel->vector_ = { 1.0f,0.0f,0.0f };
				}
			}
			break;
			}
		}
		if (ImGui::TreeNode("Root")) {
			ImGui::DragFloat("全体フレーム", &jsonData_.rootData.allFrame, 0.1f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Hook")) {
			ImGui::DragFloat("全体フレーム", &jsonData_.attackData.allFrame, 0.1f, 0.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("LowerAttack")) {
			ImGui::DragFloat3("Position", &jsonData_.lowerAttackData.position.x, 0.1f);
			ImGui::DragFloat3("scale", &jsonData_.lowerAttackData.scale.x, 0.1f);
			ImGui::DragFloat("chargeEasingTime", &jsonData_.lowerAttackData.chargeEasingTime, 0.1f);
			ImGui::DragFloat("attackEasingTime", &jsonData_.lowerAttackData.attackEasingTime, 0.1f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("InsideAttack")) {
			ImGui::DragFloat3("Position", &jsonData_.insideAttackData.position.x, 0.1f);
			ImGui::DragFloat3("scale", &jsonData_.insideAttackData.scale.x, 0.1f);
			ImGui::DragFloat("chargeEasingTime", &jsonData_.insideAttackData.chargeEasingTime, 0.1f);
			ImGui::DragFloat("attackEasingTime", &jsonData_.insideAttackData.attackEasingTime, 0.1f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("BeamAttack")) {
			ImGui::DragFloat3("startPosition", &jsonData_.beamAttackData.startPosition.x, 0.1f);
			ImGui::DragFloat3("endPosition", &jsonData_.beamAttackData.endPosition.x, 0.1f);
			ImGui::DragFloat3("scale", &jsonData_.beamAttackData.scale.x, 0.1f);
			ImGui::DragFloat("chargeEasingTime", &jsonData_.beamAttackData.chargeEasingTime, 0.1f);
			ImGui::DragFloat("attackEasingTime", &jsonData_.beamAttackData.attackEasingTime, 0.1f);
			ImGui::TreePop();
		}
		activeState_->SetDesc();
		if (ImGui::Button("Save")) {
			JSON_OPEN("Resources/Data/Boss/Boss.json");
			JSON_OBJECT("StateRoot");
			JSON_SAVE(jsonData_.rootData.allFrame);
			JSON_ROOT();
			JSON_OBJECT("StateHook");
			JSON_SAVE(jsonData_.attackData.allFrame);
			JSON_ROOT();
			JSON_OBJECT("StateLowerAttack");
			JSON_SAVE(jsonData_.lowerAttackData.position);
			JSON_SAVE(jsonData_.lowerAttackData.scale);
			JSON_SAVE(jsonData_.lowerAttackData.chargeEasingTime);
			JSON_SAVE(jsonData_.lowerAttackData.attackEasingTime);
			JSON_ROOT();
			JSON_OBJECT("StateInsideAttack");
			JSON_SAVE(jsonData_.insideAttackData.position);
			JSON_SAVE(jsonData_.insideAttackData.scale);
			JSON_SAVE(jsonData_.insideAttackData.chargeEasingTime);
			JSON_SAVE(jsonData_.insideAttackData.attackEasingTime);
			JSON_ROOT();
			JSON_OBJECT("StateBeamAttack");
			JSON_SAVE(jsonData_.beamAttackData.startPosition);
			JSON_SAVE(jsonData_.beamAttackData.endPosition);
			JSON_SAVE(jsonData_.beamAttackData.scale);
			JSON_SAVE(jsonData_.beamAttackData.chargeEasingTime);
			JSON_SAVE(jsonData_.beamAttackData.attackEasingTime);
			JSON_ROOT();
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
}

void BossStateManager::ChangeState(const BossStateManager::State& state) {
	state_ = state;
	switch (state) {
	case BossStateManager::kRoot:
		standbyState_ = std::make_unique<BossStateRoot>(*this);
		break;
	case BossStateManager::kHook:
		standbyState_ = std::make_unique<BossStateHook>(*this);
		break;
	case BossStateManager::kLowerAttack:
		standbyState_ = std::make_unique<BossStateLowerAttack>(*this);
		break;
	case BossStateManager::kInsideAttack:
		standbyState_ = std::make_unique<BossStateInsideAttack>(*this);
		break;
	case BossStateManager::kBeamAttack:
		standbyState_ = std::make_unique<BossStateBeamAttack>(*this);
		break;
	default:
		break;
	}
}

void BossStateRoot::Initialize() {
	SetDesc();
	time_ = 0.0f;
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->SetIsAlive(false);
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->SetIsAlive(false);
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->SetIsAlive(false);
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLaser)->SetIsAlive(false);
}

void BossStateRoot::SetDesc() {
	data_ = manager_.jsonData_.rootData;
}

void BossStateRoot::Update() {
	float t = time_ / data_.allFrame;
	auto& skeleton = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetSkeleton();
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kRoot);
	skeleton->ApplyAnimation(parts.animation->GetAnimation("move"), t);
	skeleton->Update();
	parts.UpdateCollider(manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->transform.worldMatrix, *skeleton.get());
	time_ += 1.0f;
	time_ = std::fmodf(time_, data_.allFrame);
}

void BossStateRoot::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void BossStateHook::Initialize() {
	SetDesc();
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kHook).SetIsCollision(true);
	time_ = 0.0f;
}

void BossStateHook::SetDesc() {
	data_ = manager_.jsonData_.attackData;
}

void BossStateHook::Update() {
	float t = time_ / data_.allFrame;
	auto& skeleton = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetSkeleton();
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kHook);
	skeleton->ApplyAnimation(parts.animation->GetAnimation("armAttack"), t);
	skeleton->Update();

	parts.UpdateCollider(manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->transform.worldMatrix, *skeleton.get());

	time_ += 1.0f;
	if (t >= 1.0f) {
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kHook).SetIsCollision(false);
		//manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetModel()->SetSkeleton(nullptr);
		manager_.ChangeState(BossStateManager::State::kRoot);
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
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->transform.scale = data_.scale;
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
	float t = time_ / data_.chargeEasingTime;
	time_ += 1.0f;

	auto& skeleton = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetSkeleton();
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kArmHammer);
	skeleton->ApplyAnimation(parts.animation->GetAnimation("armHanmar"), t);
	skeleton->Update();

	parts.UpdateCollider(manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->transform.worldMatrix, *skeleton.get());
	if (t >= 1.0f) {
		auto& floorAllTransform = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->transform;
		floorAllTransform.translate = data_.position;
		attackState_ = kAttack;
		time_ = 0.0f;
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->SetIsAlive(true);
	}
}

void BossStateLowerAttack::AttackUpdate() {
	float t = time_ / data_.chargeEasingTime;
	time_ += 1.0f;
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->GetModel()->SetColor({ 1.0f,0.0f,0.0f });
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->transform.rotate = Quaternion::MakeForYAxis(rnd_.NextFloatRange(-1.0f, 1.0f));
	auto& rotate = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->GetRotate();
	rotate.y += 1.0f;
	if (t >= 1.0f) {
		rotate.y = 0.0f;
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->SetIsAlive(false);
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->transform.rotate = Quaternion::identity;
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->GetModel()->SetColor({ 1.0f,1.0f,1.0f });
		manager_.ChangeState(BossStateManager::State::kRoot);
	}
}

void BossStateInsideAttack::Initialize() {
	SetDesc();
	attackState_ = kChage;
	time_ = 0.0f;
}

void BossStateInsideAttack::SetDesc() {
	data_ = manager_.jsonData_.insideAttackData;
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->transform.scale = data_.scale;
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->SetIsAlive(false);
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLaser)->SetModelIsAlive(true);
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
	float t = time_ / data_.chargeEasingTime;
	time_ += 1.0f;

	auto& skeleton = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetSkeleton();
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kLazerAttack);
	skeleton->ApplyAnimation(parts.animation->GetAnimation("razerAttack"), t);
	skeleton->Update();
	parts.UpdateCollider(manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->transform.worldMatrix, *skeleton.get());

	auto& laserTransform = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLaser)->transform;
	laserTransform.scale = Vector3::Lerp(t, Vector3(1.0f, 0.0f, 1.0f), Vector3::one);
	Vector3 modelSize = (manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLaser)->GetModel()->GetModel()->GetMeshes().at(0).maxVertex - manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLaser)->GetModel()->GetModel()->GetMeshes().at(0).minVertex);
	laserTransform.translate = Vector3::Lerp(t, { 0.0f,modelSize.y,modelSize.z }, Vector3::zero);
	if (t >= 1.0f) {
		auto& longDistanceAttackTransform = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->transform;
		longDistanceAttackTransform.translate = data_.position;
		attackState_ = kAttack;
		time_ = 0.0f;
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->SetIsAlive(true);
	}
}

void BossStateInsideAttack::AttackUpdate() {
	float t = time_ / data_.attackEasingTime;
	time_ += 1.0f;
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->GetModel()->SetColor({ 1.0f,0.0f,0.0f });
	auto& rotate = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->GetRotate();
	rotate.y += 1.0f;
	if (t >= 1.0f) {
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLaser)->SetIsAlive(false);
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->SetIsAlive(false);
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->GetModel()->SetColor({ 1.0f,1.0f,1.0f });
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->transform.rotate = Quaternion::identity;
		rotate.y = 0.0f;
		manager_.ChangeState(BossStateManager::State::kRoot);
	}
	else {
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->transform.scale= Vector3::one;
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->SetIsAlive(true);
	}
}

void BossStateBeamAttack::Initialize() {
	SetDesc();
	attackState_ = kChage;
	time_ = 0.0f;
}

void BossStateBeamAttack::SetDesc() {
	data_ = manager_.jsonData_.beamAttackData;
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->transform.scale = data_.scale;
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->SetModelIsAlive(true);
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->SetColliderIsAlive(false);
}

void BossStateBeamAttack::Update() {
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

void BossStateBeamAttack::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void BossStateBeamAttack::ChargeUpdate() {
	// BossModelManager を取得
	auto& tmp = manager_.boss.GetModelManager()->GetModel(BossParts::kBeamAttack);
	if (!tmp) {
		// エラーハンドリング: bossModelManager が null の場合
		//assert(0);
	}
	// BeamAttack 型にキャスト
	BeamAttack* beamAttackModel = dynamic_cast<BeamAttack*>(tmp.get());
	Vector3 vector{};
	if (beamAttackModel) {
		vector = beamAttackModel->vector_;
	}
	auto& beamAttackTransform = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->transform;
	float t = time_ / data_.chargeEasingTime;
	time_ += 1.0f;
	beamAttackTransform.translate.x = std::lerp(data_.startPosition.x * vector.x, data_.endPosition.x * vector.x, t);
	beamAttackTransform.translate.y = std::lerp(data_.startPosition.y, data_.endPosition.y, t);
	beamAttackTransform.translate.z = std::lerp(data_.startPosition.z, data_.endPosition.z, t);

	if (t >= 1.0f) {
		attackState_ = kAttack;
		time_ = 0.0f;
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->SetColliderIsAlive(true);
	}
}

void BossStateBeamAttack::AttackUpdate() {
	float t = time_ / data_.chargeEasingTime;
	time_ += 1.0f;
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->GetModel()->SetColor({ 1.0f,0.0f,0.0f });
	if (t >= 1.0f) {
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->SetIsAlive(false);
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->GetModel()->SetColor({ 1.0f,1.0f,1.0f });
		manager_.ChangeState(BossStateManager::State::kRoot);
	}
}
