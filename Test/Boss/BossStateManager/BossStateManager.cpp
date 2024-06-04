#include "BossStateManager.h"

#include "CharacterState.h"
#include "Boss/Boss.h"
#include "Player/Player.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"
#include "GameSpeed.h"
#include "BossBulletManager.h"
#include "WindManager.h"

void BossStateManager::Initialize() {
	JSON_OPEN("Resources/Data/Boss/Boss.json");
	JSON_OBJECT("StateRoot");
	JSON_LOAD(jsonData_.rootData.allFrame);
	JSON_LOAD(jsonData_.rootData.transitionFrame);
	JSON_ROOT();
	JSON_OBJECT("StateHook");
	JSON_LOAD(jsonData_.attackData.allFrame);
	JSON_ROOT();
	JSON_OBJECT("StateLowerAttack");
	JSON_LOAD(jsonData_.lowerAttackData.position);
	JSON_LOAD(jsonData_.lowerAttackData.scale);
	JSON_LOAD(jsonData_.lowerAttackData.chargeEasingTime);
	JSON_LOAD(jsonData_.lowerAttackData.attackEasingTime);
	JSON_LOAD(jsonData_.lowerAttackData.transitionFrame);
	JSON_ROOT();
	JSON_OBJECT("StateInsideAttack");
	JSON_LOAD(jsonData_.insideAttackData.position);
	JSON_LOAD(jsonData_.insideAttackData.scale);
	JSON_LOAD(jsonData_.insideAttackData.chargeEasingTime);
	JSON_LOAD(jsonData_.insideAttackData.attackEasingTime);
	JSON_LOAD(jsonData_.insideAttackData.transitionFrame);
	JSON_ROOT();
	JSON_OBJECT("StateBeamAttack");
	JSON_LOAD(jsonData_.beamAttackData.position);
	JSON_LOAD(jsonData_.beamAttackData.scale);
	JSON_LOAD(jsonData_.beamAttackData.rotateEasingTime);
	JSON_LOAD(jsonData_.beamAttackData.chargeEasingTime);
	JSON_LOAD(jsonData_.beamAttackData.attackEasingTime);
	JSON_LOAD(jsonData_.beamAttackData.transitionFrame);
	JSON_LOAD(jsonData_.beamAttackData.wind.offset);
	JSON_LOAD(jsonData_.beamAttackData.wind.velocity.min);
	JSON_LOAD(jsonData_.beamAttackData.wind.velocity.max);
	JSON_LOAD(jsonData_.beamAttackData.wind.startScale.min);
	JSON_LOAD(jsonData_.beamAttackData.wind.startScale.max);
	JSON_LOAD(jsonData_.beamAttackData.wind.endScale.min);
	JSON_LOAD(jsonData_.beamAttackData.wind.endScale.max);
	JSON_LOAD(jsonData_.beamAttackData.wind.rotate);
	JSON_LOAD(jsonData_.beamAttackData.wind.lifeTime);
	JSON_LOAD(jsonData_.beamAttackData.wind.interval);
	JSON_ROOT();
	JSON_OBJECT("StateShotAttack");
	JSON_LOAD(jsonData_.shotAttackData.rotateEasingTime);
	JSON_LOAD(jsonData_.shotAttackData.chargeEasingTime);
	JSON_LOAD(jsonData_.shotAttackData.attackEasingTime);
	JSON_LOAD(jsonData_.shotAttackData.transitionFrame);
	JSON_LOAD(jsonData_.shotAttackData.numBullet);
	JSON_LOAD(jsonData_.shotAttackData.offset);
	JSON_LOAD(jsonData_.shotAttackData.range);
	JSON_LOAD(jsonData_.shotAttackData.velocity);
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
		const char* items[] = { "Root", "Hook" ,"InsideAttack","LowerAttack" ,"BeamAttack","ShotAttack" };
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
					beamAttackModel->vector_ = { 0.0f,0.0f,-1.0f };
				}
			}
			break;
			case State::kShotAttack:
			{
				ChangeState(State::kShotAttack);
			}
			break;
			}
		}
		if (ImGui::TreeNode("Root")) {
			ImGui::DragFloat("全体フレーム", &jsonData_.rootData.allFrame, 0.1f);
			ImGui::DragFloat("遷移フレーム", &jsonData_.rootData.transitionFrame, 0.1f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Hook")) {
			ImGui::DragFloat("全体フレーム", &jsonData_.attackData.allFrame, 0.1f, 0.0f);
			ImGui::DragFloat("遷移フレーム", &jsonData_.attackData.transitionFrame, 0.1f, 0.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("LowerAttack")) {
			ImGui::DragFloat3("Position", &jsonData_.lowerAttackData.position.x, 0.1f);
			ImGui::DragFloat3("scale", &jsonData_.lowerAttackData.scale.x, 0.1f);
			ImGui::DragFloat("chargeEasingTime", &jsonData_.lowerAttackData.chargeEasingTime, 0.1f);
			ImGui::DragFloat("attackEasingTime", &jsonData_.lowerAttackData.attackEasingTime, 0.1f);
			ImGui::DragFloat("遷移フレーム", &jsonData_.lowerAttackData.transitionFrame, 0.1f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("InsideAttack")) {
			ImGui::DragFloat3("Position", &jsonData_.insideAttackData.position.x, 0.1f);
			ImGui::DragFloat3("scale", &jsonData_.insideAttackData.scale.x, 0.1f);
			ImGui::DragFloat("chargeEasingTime", &jsonData_.insideAttackData.chargeEasingTime, 0.1f);
			ImGui::DragFloat("attackEasingTime", &jsonData_.insideAttackData.attackEasingTime, 0.1f);
			ImGui::DragFloat("遷移フレーム", &jsonData_.insideAttackData.transitionFrame, 0.1f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("BeamAttack")) {
			ImGui::DragFloat3("position", &jsonData_.beamAttackData.position.x, 0.1f);
			ImGui::DragFloat3("scale", &jsonData_.beamAttackData.scale.x, 0.1f);
			ImGui::DragFloat("rotateEasingTime", &jsonData_.beamAttackData.rotateEasingTime, 0.1f);
			ImGui::DragFloat("chargeEasingTime", &jsonData_.beamAttackData.chargeEasingTime, 0.1f);
			ImGui::DragFloat("attackEasingTime", &jsonData_.beamAttackData.attackEasingTime, 0.1f);
			ImGui::DragFloat("遷移フレーム", &jsonData_.beamAttackData.transitionFrame, 0.1f);
			if (ImGui::TreeNode("風")) {
				ImGui::DragFloat3("offset", &jsonData_.beamAttackData.wind.offset.x, 0.1f);
				ImGui::DragFloat("velocity : min ", &jsonData_.beamAttackData.wind.velocity.min, 0.1f);
				float min = jsonData_.beamAttackData.wind.velocity.min;
				ImGui::DragFloat("velocity : max ", &jsonData_.beamAttackData.wind.velocity.max, 0.1f, min);
				ImGui::DragFloat("startScale : min ", &jsonData_.beamAttackData.wind.startScale.min, 0.1f, 0.0f);
				min = jsonData_.beamAttackData.wind.startScale.min;
				ImGui::DragFloat("startScale : max ", &jsonData_.beamAttackData.wind.startScale.max, 0.1f, min);
				ImGui::DragFloat("endScale : min ", &jsonData_.beamAttackData.wind.endScale.min, 0.1f, 0.0f);
				min = jsonData_.beamAttackData.wind.endScale.min;
				ImGui::DragFloat("endScale : max ", &jsonData_.beamAttackData.wind.endScale.max, 0.1f, min);
				ImGui::DragFloat("rotate", &jsonData_.beamAttackData.wind.rotate, 0.1f);
				int lifeTime = jsonData_.beamAttackData.wind.lifeTime;
				ImGui::DragInt("lifeTime", &lifeTime, 1, 0);
				jsonData_.beamAttackData.wind.lifeTime = lifeTime;
				lifeTime = jsonData_.beamAttackData.wind.interval;
				ImGui::DragInt("間隔", &lifeTime, 1, 0);
				jsonData_.beamAttackData.wind.interval = lifeTime;
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("ShotAttack")) {
			ImGui::DragFloat3("offset", &jsonData_.shotAttackData.offset.x, 0.1f);
			ImGui::DragFloat("rotateEasingTime", &jsonData_.shotAttackData.rotateEasingTime, 0.1f);
			ImGui::DragFloat("chargeEasingTime", &jsonData_.shotAttackData.chargeEasingTime, 0.1f);
			ImGui::DragFloat("attackEasingTime", &jsonData_.shotAttackData.attackEasingTime, 0.1f);
			ImGui::DragFloat("弾打つ幅", &jsonData_.shotAttackData.range, 0.1f, 0.0f);
			ImGui::DragFloat("弾打つ速度", &jsonData_.shotAttackData.velocity, 0.1f, 0.0f);
			int num = jsonData_.shotAttackData.numBullet;
			ImGui::DragInt("attackEasingTime中に打つ弾の数", &num, 1, 0);
			jsonData_.shotAttackData.numBullet = num;
			ImGui::DragFloat("遷移フレーム", &jsonData_.shotAttackData.transitionFrame, 0.1f);
			ImGui::TreePop();
		}
		activeState_->SetDesc();
		if (ImGui::Button("Save")) {
			JSON_OPEN("Resources/Data/Boss/Boss.json");
			JSON_OBJECT("StateRoot");
			JSON_SAVE(jsonData_.rootData.allFrame);
			JSON_SAVE(jsonData_.rootData.transitionFrame);
			JSON_ROOT();
			JSON_OBJECT("StateHook");
			JSON_SAVE(jsonData_.attackData.allFrame);
			JSON_SAVE(jsonData_.attackData.transitionFrame);
			JSON_ROOT();
			JSON_OBJECT("StateLowerAttack");
			JSON_SAVE(jsonData_.lowerAttackData.position);
			JSON_SAVE(jsonData_.lowerAttackData.scale);
			JSON_SAVE(jsonData_.lowerAttackData.chargeEasingTime);
			JSON_SAVE(jsonData_.lowerAttackData.attackEasingTime);
			JSON_SAVE(jsonData_.lowerAttackData.transitionFrame);
			JSON_ROOT();
			JSON_OBJECT("StateInsideAttack");
			JSON_SAVE(jsonData_.insideAttackData.position);
			JSON_SAVE(jsonData_.insideAttackData.scale);
			JSON_SAVE(jsonData_.insideAttackData.chargeEasingTime);
			JSON_SAVE(jsonData_.insideAttackData.attackEasingTime);
			JSON_SAVE(jsonData_.insideAttackData.transitionFrame);
			JSON_ROOT();
			JSON_OBJECT("StateBeamAttack");
			JSON_SAVE(jsonData_.beamAttackData.position);
			JSON_SAVE(jsonData_.beamAttackData.scale);
			JSON_SAVE(jsonData_.beamAttackData.rotateEasingTime);
			JSON_SAVE(jsonData_.beamAttackData.chargeEasingTime);
			JSON_SAVE(jsonData_.beamAttackData.attackEasingTime);
			JSON_SAVE(jsonData_.beamAttackData.wind.offset);
			JSON_SAVE(jsonData_.beamAttackData.wind.velocity.min);
			JSON_SAVE(jsonData_.beamAttackData.wind.velocity.max);
			JSON_SAVE(jsonData_.beamAttackData.wind.startScale.min);
			JSON_SAVE(jsonData_.beamAttackData.wind.startScale.max);
			JSON_SAVE(jsonData_.beamAttackData.wind.endScale.min);
			JSON_SAVE(jsonData_.beamAttackData.wind.endScale.max);
			JSON_SAVE(jsonData_.beamAttackData.wind.rotate);
			JSON_SAVE(jsonData_.beamAttackData.wind.lifeTime);
			JSON_SAVE(jsonData_.beamAttackData.wind.interval);
			JSON_ROOT();
			JSON_OBJECT("StateShotAttack");
			JSON_SAVE(jsonData_.shotAttackData.rotateEasingTime);
			JSON_SAVE(jsonData_.shotAttackData.chargeEasingTime);
			JSON_SAVE(jsonData_.shotAttackData.attackEasingTime);
			JSON_SAVE(jsonData_.shotAttackData.numBullet);
			JSON_SAVE(jsonData_.shotAttackData.offset);
			JSON_SAVE(jsonData_.shotAttackData.range);
			JSON_SAVE(jsonData_.shotAttackData.velocity);
			JSON_SAVE(jsonData_.shotAttackData.transitionFrame);
			JSON_ROOT();
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
}

void BossStateManager::ChangeState(const BossStateManager::State& state) {
	bool inTransition = false;
	if (activeState_) {
		prevAnimation_ = activeState_->GetAnimation();
		prevAnimationTime_ = activeState_->GetAnimationTime();
		if (prevAnimation_) {
			inTransition = true;
		}
	}
	state_ = state;
	switch (state) {
	case BossStateManager::kRoot:
		standbyState_ = std::make_unique<BossStateRoot>(*this, inTransition);
		break;
	case BossStateManager::kHook:
		standbyState_ = std::make_unique<BossStateHook>(*this, inTransition);
		break;
	case BossStateManager::kLowerAttack:
		standbyState_ = std::make_unique<BossStateLowerAttack>(*this, inTransition);
		break;
	case BossStateManager::kInsideAttack:
		standbyState_ = std::make_unique<BossStateInsideAttack>(*this, inTransition);
		break;
	case BossStateManager::kBeamAttack:
		standbyState_ = std::make_unique<BossStateBeamAttack>(*this, inTransition);
		break;
	case BossStateManager::kShotAttack:
		standbyState_ = std::make_unique<BossStateShotAttack>(*this, inTransition);
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
	time_ += 1.0f;
	if (inTransition_ && time_ >= data_.transitionFrame) {
		inTransition_ = false;
		time_ -= data_.transitionFrame;
	}
	if (!inTransition_) {
		time_ = std::fmod(time_, data_.allFrame);
	}

	auto& skeleton = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetSkeleton();
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kRoot);
	if (inTransition_) {
		float t = time_ / data_.transitionFrame;
		skeleton->ApplyAnimationTransition(*manager_.GetPrevAnimation(), manager_.GetPrevAnimationTime(), parts.animation->GetAnimation("move"), 0.0f, t);
	}
	else {
		float t = time_ / data_.allFrame;
		skeleton->ApplyAnimation(parts.animation->GetAnimation("move"), t);
	}
	skeleton->Update();
	parts.UpdateCollider(manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->transform.worldMatrix, *skeleton.get());
}

void BossStateRoot::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

AnimationSet* BossStateRoot::GetAnimation() const {
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kRoot);
	return &(parts.animation->GetAnimation("move"));
}

float BossStateRoot::GetAnimationTime() const {
	return time_ / data_.allFrame;
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
	time_ += 1.0f;
	if (inTransition_ && time_ >= data_.transitionFrame) {
		inTransition_ = false;
		time_ -= data_.transitionFrame;
	}
	if (!inTransition_) {
		if (time_ >= data_.allFrame) {
			manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kHook).SetIsCollision(false);
			manager_.ChangeState(BossStateManager::State::kRoot);
			time_ = data_.allFrame;
		}
	}

	auto& skeleton = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetSkeleton();
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kHook);
	if (inTransition_) {
		float t = time_ / data_.transitionFrame;
		skeleton->ApplyAnimationTransition(*manager_.GetPrevAnimation(), manager_.GetPrevAnimationTime(), parts.animation->GetAnimation("armAttack"), 0.0f, t);
	}
	else {
		float t = time_ / data_.allFrame;
		skeleton->ApplyAnimation(parts.animation->GetAnimation("armAttack"), t);
	}
	skeleton->Update();
	parts.UpdateCollider(manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->transform.worldMatrix, *skeleton.get());
}

void BossStateHook::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

AnimationSet* BossStateHook::GetAnimation() const {
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kHook);
	return &(parts.animation->GetAnimation("armAttack"));
}

float BossStateHook::GetAnimationTime() const {
	return time_ / data_.allFrame;
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

AnimationSet* BossStateLowerAttack::GetAnimation() const {
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kArmHammer);
	return &parts.animation->GetAnimation("armHanmar");
}

float BossStateLowerAttack::GetAnimationTime() const {
	switch (attackState_) {
	case BossState::kChage:
		return time_ / data_.chargeEasingTime;
	case BossState::kAttack:
		return 1.0f;
	}
	return 0.0f;
}

void BossStateLowerAttack::ChargeUpdate() {
	time_ += 1.0f;
	if (inTransition_ && time_ >= data_.transitionFrame) {
		inTransition_ = false;
		time_ -= data_.transitionFrame;
	}

	auto& skeleton = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetSkeleton();
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kArmHammer);
	if (inTransition_) {
		float t = time_ / data_.transitionFrame;
		skeleton->ApplyAnimationTransition(*manager_.GetPrevAnimation(), manager_.GetPrevAnimationTime(), parts.animation->GetAnimation("armHanmar"), 0.0f, t);
	}
	else {
		float t = time_ / data_.chargeEasingTime;
		skeleton->ApplyAnimation(parts.animation->GetAnimation("armHanmar"), t);
	}
	skeleton->Update();
	parts.UpdateCollider(manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->transform.worldMatrix, *skeleton.get());

	if (!inTransition_) {
		if (time_ >= data_.chargeEasingTime) {
			auto& floorAllTransform = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->transform;
			floorAllTransform.translate = data_.position;
			attackState_ = kAttack;
			time_ = 0.0f;
			manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->SetIsAlive(true);
		}
	}
}

void BossStateLowerAttack::AttackUpdate() {
	float t = time_ / data_.attackEasingTime;
	time_ += 1.0f;
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->GetModel()->SetColor({ 1.0f,1.0f,1.0f });
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->transform.rotate = Quaternion::MakeForYAxis(rnd_.NextFloatRange(-1.0f, 1.0f));
	auto& rotate = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kFloorAll)->GetRotate();
	rotate.y += 0.5f;
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

AnimationSet* BossStateInsideAttack::GetAnimation() const {
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kLazerAttack);
	return &parts.animation->GetAnimation("razerAttack");
}

float BossStateInsideAttack::GetAnimationTime() const {
	switch (attackState_) {
	case BossState::kChage:
		return time_ / data_.chargeEasingTime;
	case BossState::kAttack:
		return 1.0f;
	}
	return 0.0f;
}

void BossStateInsideAttack::ChargeUpdate() {
	time_ += 1.0f;
	if (inTransition_ && time_ >= data_.transitionFrame) {
		inTransition_ = false;
		time_ -= data_.transitionFrame;
	}

	auto& skeleton = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetSkeleton();
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kLazerAttack);
	if (inTransition_) {
		float t = time_ / data_.transitionFrame;
		skeleton->ApplyAnimationTransition(*manager_.GetPrevAnimation(), manager_.GetPrevAnimationTime(), parts.animation->GetAnimation("razerAttack"), 0.0f, t);
	}
	else {
		float t = time_ / data_.chargeEasingTime;
		if (t < 0.5f) {
			t = (1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) * 0.5f;

		}
		else {
			t = (std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) * 0.5f;
		}

		skeleton->ApplyAnimation(parts.animation->GetAnimation("razerAttack"), t);
		auto& laserTransform = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLaser)->transform;
		laserTransform.scale = Vector3::Lerp(t, Vector3(1.0f, 0.0f, 1.0f), Vector3::one);
		Vector3 modelSize = (manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLaser)->GetModel()->GetModel()->GetMeshes().at(0).maxVertex - manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLaser)->GetModel()->GetModel()->GetMeshes().at(0).minVertex);
		laserTransform.translate = Vector3::Lerp(t, { 0.0f,modelSize.y,modelSize.z }, Vector3::zero);
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLaser)->SetModelIsAlive(true);
	}
	skeleton->Update();
	parts.UpdateCollider(manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->transform.worldMatrix, *skeleton.get());

	if (!inTransition_) {
		if (time_ >= data_.chargeEasingTime) {
			auto& longDistanceAttackTransform = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->transform;
			longDistanceAttackTransform.translate = data_.position;
			attackState_ = kAttack;
			time_ = 0.0f;
			manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->SetIsAlive(true);
		}
	}

}

void BossStateInsideAttack::AttackUpdate() {
	float t = time_ / data_.attackEasingTime;
	time_ += 1.0f;
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->GetModel()->SetColor({ 1.0f,1.0f,1.0f });
	auto& rotate = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->GetRotate();
	rotate.y += 0.5f;
	if (t >= 1.0f) {
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLaser)->SetIsAlive(false);
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->SetIsAlive(false);
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->GetModel()->SetColor({ 1.0f,1.0f,1.0f });
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->transform.rotate = Quaternion::identity;
		rotate.y = 0.0f;
		manager_.ChangeState(BossStateManager::State::kRoot);
	}
	else {
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->transform.scale = Vector3::one;
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kLongDistanceAttack)->SetIsAlive(true);
	}
}

void BossStateBeamAttack::Initialize() {
	SetDesc();
	attackState_ = kRotate;
	time_ = 0.0f;
	lastWindTime_ = 0.0f;
}

void BossStateBeamAttack::SetDesc() {
	data_ = manager_.jsonData_.beamAttackData;
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->transform.scale = data_.scale;
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->SetIsAlive(false);
	// BossModelManager を取得
	auto& tmp = manager_.boss.GetModelManager()->GetModel(BossParts::kBeamAttack);
	if (!tmp) {
		// エラーハンドリング: bossModelManager が null の場合
		//assert(0);
	}
	// BeamAttack 型にキャスト
	BeamAttack* beamAttackModel = dynamic_cast<BeamAttack*>(tmp.get());
	if (beamAttackModel) {
		data_.vector = beamAttackModel->vector_;
	}
}

void BossStateBeamAttack::Update() {
	switch (attackState_) {
	case BossState::kRotate:
	{
		RotateUpdate();
	}
	break;
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

AnimationSet* BossStateBeamAttack::GetAnimation() const {
	return nullptr;
}

float BossStateBeamAttack::GetAnimationTime() const {
	switch (attackState_) {
	case BossState::kRotate:
		return 0.0f;
	case BossState::kChage:
		return time_ / data_.chargeEasingTime;
	case BossState::kAttack:
		return 1.0f;
	}
	return 0.0f;
}

void BossStateBeamAttack::ChargeUpdate() {
	time_ += 1.0f;
	if (inTransition_ && time_ >= data_.transitionFrame) {
		inTransition_ = false;
		time_ -= data_.transitionFrame;
	}

	auto& skeleton = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetSkeleton();
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kWindAttack);
	if (inTransition_) {
		float t = time_ / data_.transitionFrame;
		skeleton->ApplyAnimationTransition(*manager_.GetPrevAnimation(), manager_.GetPrevAnimationTime(), parts.animation->GetAnimation("WindAttack"), 0.0f, t);
	}
	else {
		float t = time_ / data_.chargeEasingTime;
		skeleton->ApplyAnimation(parts.animation->GetAnimation("WindAttack"), t);
	}
	skeleton->Update();
	parts.UpdateCollider(manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->transform.worldMatrix, *skeleton.get());

	if (!inTransition_) {
		if (time_ >= data_.chargeEasingTime) {
			attackState_ = kAttack;
			time_ = 0.0f;
			manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->SetColliderIsAlive(true);
			manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->transform.translate = data_.position;
			manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->transform.scale = data_.scale;
			// 一個だけ生成
			auto manager = WindManager::GetInstance();
			Wind::Desc desc{};
			desc.position = manager_.boss.transform.worldMatrix.GetTranslate() + data_.wind.offset;
			desc.velocity = data_.vector * rnd_.NextFloatRange(data_.wind.velocity.min, data_.wind.velocity.max);
			float scale = rnd_.NextFloatRange(data_.wind.startScale.min, data_.wind.startScale.max);
			desc.scale.start = { scale ,scale ,scale };
			scale = rnd_.NextFloatRange(data_.wind.endScale.min, data_.wind.endScale.max);
			desc.scale.end = { scale ,scale ,scale };
			desc.rotate.z = rnd_.NextFloatRange(-data_.wind.rotate, data_.wind.rotate) * Math::ToRadian;
			if (desc.rotate.z == 0.0f) {
				desc.rotate.z = -1.0f * Math::ToRadian;
			}
			desc.lifeTime = data_.wind.lifeTime;
			manager->Create(desc);
		}
	}
}

void BossStateBeamAttack::AttackUpdate() {
	float t = time_ / data_.attackEasingTime;
	time_ += 1.0f;
	manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->GetModel()->SetColor({ 1.0f,1.0f,1.0f });
	static const uint32_t kNumWind = 10;

	if (time_ >= lastWindTime_ + data_.wind.interval) {
		auto manager = WindManager::GetInstance();
		Wind::Desc desc{};
		desc.position = manager_.boss.transform.worldMatrix.GetTranslate() + data_.wind.offset;
		desc.velocity = data_.vector * rnd_.NextFloatRange(data_.wind.velocity.min, data_.wind.velocity.max);
		float scale = rnd_.NextFloatRange(data_.wind.startScale.min, data_.wind.startScale.max);
		desc.scale.start = { scale ,scale ,scale };
		scale = rnd_.NextFloatRange(data_.wind.endScale.min, data_.wind.endScale.max);
		desc.scale.end = { scale ,scale ,scale };
		desc.rotate.z = rnd_.NextFloatRange(-data_.wind.rotate, data_.wind.rotate) * Math::ToRadian;
		if (desc.rotate.z == 0.0f) {
			desc.rotate.z = -1.0f * Math::ToRadian;
		}
		desc.lifeTime = data_.wind.lifeTime;
		manager->Create(desc);
		lastWindTime_ += data_.wind.interval;
	}

	if (t >= 1.0f) {
		auto& rotate = manager_.boss.transform.rotate;
		rotate = Quaternion::MakeForYAxis(180.0f * Math::ToRadian);
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->SetIsAlive(false);
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->GetModel()->SetColor({ 1.0f,1.0f,1.0f });
		manager_.ChangeState(BossStateManager::State::kRoot);
	}
}


void BossStateShotAttack::Initialize() {
	SetDesc();
	attackState_ = kRotate;
	time_ = 0.0f;
	lastBulletTime_ = 0.0f;
}

void BossStateShotAttack::SetDesc() {
	data_ = manager_.jsonData_.shotAttackData;
}

void BossStateShotAttack::Update() {
	switch (attackState_) {
	case BossState::kRotate:
	{
		RotateUpdate();
	}
	break;
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

void BossStateShotAttack::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

AnimationSet* BossStateShotAttack::GetAnimation() const {
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kShotAttack);
	return &parts.animation->GetAnimation("shotAttack");
}

float BossStateShotAttack::GetAnimationTime() const {
	switch (attackState_) {
	case BossState::kRotate:
		return 0.0f;
	case BossState::kChage:
		return time_ / data_.chargeEasingTime;
	case BossState::kAttack:
		return 1.0f;
	}
	return 0.0f;
}
void BossStateBeamAttack::RotateUpdate() {
	float t = time_ / data_.rotateEasingTime;
	time_ += 1.0f;
	auto& rotate = manager_.boss.transform.rotate;
	rotate = Quaternion::MakeForYAxis(std::lerp(0.0f * Math::ToRadian, 180.0f * Math::ToRadian, t));
	if (t >= 1.0f) {
		rotate = Quaternion::MakeForYAxis(0.0f * Math::ToRadian);
		attackState_ = kChage;
		time_ = 0.0f;
	}
}

void BossStateShotAttack::ChargeUpdate() {
	float t = time_ / data_.chargeEasingTime;
	time_ += 1.0f;
	auto& skeleton = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetSkeleton();
	auto& parts = manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->GetAnimation(BossBody::kShotAttack);
	skeleton->ApplyAnimation(parts.animation->GetAnimation("shotAttack"), t);
	skeleton->Update();
	parts.UpdateCollider(manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBossBody)->transform.worldMatrix, *skeleton.get());


	float interval = data_.attackEasingTime / data_.numBullet;

	if (time_ >= lastBulletTime_ + interval) {
		float x = (rnd_.NextIntRange(-1, 1)) * data_.range;
		BossBulletManager::GetInstance()->Create(manager_.boss.transform, Vector3(x, 0.0f, 0.0f) + data_.offset, Vector3(0.0f, 0.0f, 0.0f));
		lastBulletTime_ += interval;
	}

	if (t >= 1.0f) {
		attackState_ = kAttack;
		time_ = 0.0f;
		manager_.boss.GetModelManager()->GetModel(BossParts::Parts::kBeamAttack)->SetColliderIsAlive(true);
		lastBulletTime_ = 0.0f;
	}
}

void BossStateShotAttack::AttackUpdate() {
	time_ += 1.0f;
	float t = time_ / data_.attackEasingTime;
	float interval = data_.attackEasingTime / data_.numBullet;

	if (time_ >= lastBulletTime_ + interval) {
		for (auto& bullet : BossBulletManager::GetInstance()->GetBullets()) {
			if (bullet->GetVelocity() == 0.0f) {
				bullet->SetVelocity(data_.velocity);
				lastBulletTime_ += interval;
				break;
			}
		}
	}

	if (t >= 1.0f) {
		auto& rotate = manager_.boss.transform.rotate;
		rotate = Quaternion::MakeForYAxis(180.0f * Math::ToRadian);
		manager_.ChangeState(BossStateManager::State::kRoot);
		lastBulletTime_ = 0.0f;
	}
}

void BossStateShotAttack::RotateUpdate() {
	float t = time_ / data_.rotateEasingTime;
	time_ += 1.0f;
	auto& rotate = manager_.boss.transform.rotate;
	rotate = Quaternion::MakeForYAxis(std::lerp(180.0f * Math::ToRadian, 0.0f * Math::ToRadian, t));
	if (t >= 1.0f) {
		rotate = Quaternion::MakeForYAxis(0.0f * Math::ToRadian);
		attackState_ = kChage;
		time_ = 0.0f;
	}
}
