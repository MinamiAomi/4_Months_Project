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
	JSON_LOAD(jsonData_.attackData.startRotate);
	JSON_LOAD(jsonData_.attackData.endRotate);
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
			Vector3 rotate = jsonData_.attackData.startRotate * Math::ToDegree;
			ImGui::DragFloat3("startRotate", &rotate.x, 0.1f);
			jsonData_.attackData.startRotate = rotate * Math::ToRadian;
			rotate = jsonData_.attackData.endRotate * Math::ToDegree;
			ImGui::DragFloat3("endRotate", &rotate.x, 0.1f);
			jsonData_.attackData.endRotate = rotate * Math::ToRadian;
			ImGui::DragFloat("easingTime", &jsonData_.attackData.easingTime, 0.1f);
			ImGui::TreePop();
		}
		activeState_->SetDesc();
		if (ImGui::Button("Save")) {
			JSON_OPEN("Resources/Data/Boss/Boss.json");
			JSON_OBJECT("StateRoot");
			JSON_SAVE(jsonData_.rootData.velocity);
			JSON_ROOT();
			JSON_OBJECT("StateAtacck");
			JSON_SAVE(jsonData_.attackData.startPosition);
			JSON_SAVE(jsonData_.attackData.endPosition);
			JSON_SAVE(jsonData_.attackData.startRotate);
			JSON_SAVE(jsonData_.attackData.endRotate);
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
	SetDesc();
	auto& leftArmTransform = manager_.boss.GetModel()->GetModel(BossParts::Parts::kLeftArm)->transform;
	leftArmTransform.translate = manager_.boss.GetModel()->GetModel(BossParts::Parts::kLeftArm)->GetOffset();
	manager_.boss.GetModel()->GetModel(BossParts::Parts::kLeftArm)->SetRotate(Vector3::zero);
	leftArmTransform.rotate = Quaternion::identity;
	auto& rightArmTransform = manager_.boss.GetModel()->GetModel(BossParts::Parts::kRightArm)->transform;
	rightArmTransform.translate = manager_.boss.GetModel()->GetModel(BossParts::Parts::kRightArm)->GetOffset();
	manager_.boss.GetModel()->GetModel(BossParts::Parts::kRightArm)->SetRotate(Vector3::zero);
	rightArmTransform.rotate = Quaternion::identity;
}

void BossStateRoot::SetDesc() {
	velocity_ = manager_.jsonData_.rootData.velocity;
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
	SetDesc();
	time_ = 0.0f;
}

void BossStateAttack::SetDesc() {
	data_ = manager_.jsonData_.attackData;
}

void BossStateAttack::Update() {
	auto& transform = manager_.boss.GetModel()->GetModel(BossParts::Parts::kLeftArm)->transform;
	float t = time_ / data_.easingTime;
	time_ += 1.0f;
	transform.translate.x = std::lerp(data_.startPosition.x, data_.endPosition.x, t);
	transform.translate.y = std::lerp(data_.startPosition.y, data_.endPosition.y, t);
	transform.translate.z = std::lerp(data_.startPosition.z, data_.endPosition.z, t);
	Vector3 rotate{};
	rotate.x = std::lerp(data_.startRotate.x, data_.endRotate.x, t);
	rotate.y = std::lerp(data_.startRotate.y, data_.endRotate.y, t);
	rotate.z = std::lerp(data_.startRotate.z, data_.endRotate.z, t);
	manager_.boss.GetModel()->GetModel(BossParts::Parts::kLeftArm)->SetRotate(rotate);
	if (t >= 1.0f) {
		manager_.ChangeState<BossStateRoot>();
	}
}

void BossStateAttack::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}
