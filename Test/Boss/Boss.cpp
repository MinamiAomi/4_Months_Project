#include "Boss.h"

#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"

void Boss::Initialize() {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("boss"));
	model_->SetIsActive(true);

	transform.translate = { 0.0f,20.0f,50.0f };
	transform.rotate = Quaternion::identity;
	transform.scale = { 10.0f,10.0f,10.0f };

	isMove_ = true;
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Boss");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	collider_->SetSize({ 40.0f, 40.0f, 2.0f });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Boss);
	collider_->SetCollisionMask(~CollisionAttribute::Boss);
	collider_->SetIsActive(true);
#pragma endregion
#pragma region パラメーター
	JSON_OPEN("Resources/Data/Boss/Boss.json");
	JSON_OBJECT("Boss");
	JSON_LOAD(velocity_);
	JSON_CLOSE();
#pragma endregion
}

void Boss::Update() {
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Boss")) {
		ImGui::DragFloat3("Pos", &transform.translate.x, 0.1f);
		ImGui::DragFloat3("velocity_", &velocity_.x, 0.1f);
		if (ImGui::Button("Save")) {
			JSON_OPEN("Resources/Data/Boss/Boss.json");
			JSON_OBJECT("Boss");
			JSON_SAVE(velocity_);
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
	if (isMove_) {
		transform.translate += velocity_;
	}
	UpdateTransform();
}

void Boss::Reset() {
	transform.translate = { 0.0f,20.0f,50.0f };
	transform.rotate = Quaternion::identity;
	transform.scale = { 10.0f,10.0f,10.0f };
}

void Boss::UpdateTransform() {
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Boss::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Player") {

	}
}
