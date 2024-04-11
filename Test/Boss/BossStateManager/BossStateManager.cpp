#include "BossStateManager.h"

#include "CharacterState.h"
#include "Boss/Boss.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"

void BossStateManager::Initialize() {
	JSON_OPEN("Resources/Data/Boss/Boss.json");
	JSON_OBJECT("StateRoot");
	JSON_LOAD(jsonData_.rootData.velocity);
	JSON_ROOT();
	JSON_OBJECT("StateAtacck");
	JSON_LOAD(jsonData_.attackData.startPosition);
	JSON_LOAD(jsonData_.attackData.endPosition);
	JSON_LOAD(jsonData_.attackData.rotate);
	JSON_LOAD(jsonData_.attackData.easingTime);
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
		const char* items[] = { "Root", "Attack" };
		static int selectedItem = static_cast<int>(state_);
		if (ImGui::Combo("State", &selectedItem, items, IM_ARRAYSIZE(items))) {
			state_ = static_cast<State>(selectedItem);
			switch (state_) {
			case State::kRoot:
			{
				state_ = State::kRoot;
				ChangeState<BossStateRoot>();
			}
			break;
			case State::kAttack:
			{
				state_ = State::kAttack;
				ChangeState<BossStateAttack>();
			}
			break;
			}
		}
		if (ImGui::TreeNode("Root")) {
			ImGui::DragFloat("velocity_", &jsonData_.rootData.velocity, 0.1f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Attack")) {
			ImGui::DragFloat3("startPosition", &jsonData_.attackData.startPosition.x, 0.1f);
			ImGui::DragFloat3("endPosition", &jsonData_.attackData.endPosition.x, 0.1f);
			Vector3 rotate = jsonData_.attackData.rotate * Math::ToDegree;
			ImGui::DragFloat3("rotate", &rotate.x, 0.1f);
			jsonData_.attackData.rotate = rotate * Math::ToRadian;
			ImGui::DragFloat("easingTime", &jsonData_.attackData.easingTime, 0.1f);
			ImGui::TreePop();
		}
		activeState_->Initialize();
		if (ImGui::Button("Save")) {
			JSON_OPEN("Resources/Data/Boss/Boss.json");
			JSON_OBJECT("StateRoot");
			JSON_SAVE(jsonData_.rootData.velocity);
			JSON_ROOT();
			JSON_OBJECT("StateAtacck");
			JSON_SAVE(jsonData_.attackData.startPosition);
			JSON_SAVE(jsonData_.attackData.endPosition);
			JSON_SAVE(jsonData_.attackData.rotate);
			JSON_SAVE(jsonData_.attackData.easingTime);
			JSON_ROOT();
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
}

void BossStateRoot::Initialize() {
	velocity_ = GetManager().GetData().rootData.velocity;
}

void BossStateRoot::Update() {
	auto& transform = manager_.boss.transform;
	if (manager_.boss.GetIsMove()) {
		switch (characterState_) {
		case Character::State::kChase:
		{
			transform.translate.z += velocity_;
		}
		break;
		case Character::State::kRunAway:
		{
			transform.translate.z -= velocity_;
		}
		break;
		default:
			break;
		}
	}
}

void BossStateRoot::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void BossStateAttack::Initialize() {
	startPosition_ = manager_.jsonData_.attackData.startPosition;
	endPosition_ = manager_.jsonData_.attackData.endPosition;
	rotate_ = manager_.jsonData_.attackData.rotate;
	easingTime_ = manager_.jsonData_.attackData.easingTime;
	time_ = 0.0f;
}

void BossStateAttack::Update() {
	auto& transform = manager_.boss.GetModel()->GetModel(BossParts::Parts::kLeftArm)->transform;
	float t = time_ / easingTime_;
	transform.translate.x = std::lerp(startPosition_.x, endPosition_.x, t);
	transform.translate.y = std::lerp(startPosition_.y, endPosition_.y, t);
	transform.translate.z = std::lerp(startPosition_.z, endPosition_.z, t);
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	if (t >= 1.0f) {
		manager_.ChangeState<BossStateRoot>();
	}
}

void BossStateAttack::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}
