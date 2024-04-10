#include "Boss.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"

void Boss::Initialize() {
#pragma region パラメーター
	JSON_OPEN("Resources/Data/Boss/Boss.json");
	JSON_OBJECT("Boss");
	JSON_LOAD(velocity_);
	JSON_LOAD(offset_);
	JSON_CLOSE();
#pragma endregion

	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("boss"));
	model_->SetIsActive(true);
	Reset();
	isMove_ = true;
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Boss");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
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
		ImGui::DragFloat3("Pos", &transform.translate.x, 0.1f);
		ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
		ImGui::DragFloat3("velocity_", &velocity_.x, 0.1f);
		ImGui::DragFloat3("offset_", &offset_.x, 0.1f);
		if (ImGui::Button("Save")) {
			JSON_OPEN("Resources/Data/Boss/Boss.json");
			JSON_OBJECT("Boss");
			JSON_SAVE(velocity_);
			JSON_SAVE(offset_);
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
	if (isMove_) {
		switch (characterState_) {
		case Character::State::kChase:
		{
			transform.translate += velocity_;
		}
		break;
		case Character::State::kRunAway:
		{
			transform.translate -= velocity_;
		}
		break;
		default:
			break;
		}
	}
	UpdateTransform();
}

void Boss::Reset() {
	transform.translate = offset_;
	transform.rotate = Quaternion::identity;
	transform.scale = { 7.0f,7.0f,7.0f };
}

void Boss::UpdateTransform() {
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Boss::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Player") {

	}
}
