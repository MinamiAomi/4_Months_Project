#include "BossModelManager.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"

namespace BossParts {
	// 実際の定義
	std::array<std::string, Parts::kCount> partsName_ = {
		"bossBody",
		"bossRightArm",
		"bossLeftArm",
		"bossFloorAll",
		"bossLongDistanceAttack",
	};
}

void BossModelManager::Initialize(const Transform* Transform) {
	models_.at(BossParts::Parts::kBody) = std::make_unique<Body>();
	models_.at(BossParts::Parts::kRightArm) = std::make_unique<RightArm>();
	models_.at(BossParts::Parts::kLeftArm) = std::make_unique<LeftArm>();
	models_.at(BossParts::Parts::kFloorAll) = std::make_unique<FloorAll>();
	models_.at(BossParts::Parts::kLongDistanceAttack) = std::make_unique<LongDistanceAttack>();

	for (uint32_t i = 0; auto & model : models_) {
		model->transform.SetParent(Transform);
		model->Initialize(i);
		i++;
	}
	auto& model = models_.at(BossParts::Parts::kLeftArm);
	Vector3 modelSize = (model->GetModel()->GetModel()->GetMeshes().at(0).maxVertex - model->GetModel()->GetModel()->GetMeshes().at(0).minVertex);
	model->GetCollider()->SetSize({modelSize.x * model->transform.scale.x,modelSize.y * model->transform.scale.y * 2.0f ,modelSize.z * model->transform.scale.z});
}

void BossModelManager::Update() {
	for (auto& model : models_) {
		model->Update();
	}
}

void BossModel::Initialize(uint32_t index) {
	name_ = BossParts::partsName_.at(index);
	JSON_OPEN("Resources/Data/Boss/Boss.json");
	JSON_OBJECT(name_);
	JSON_LOAD(offset_);
	JSON_CLOSE();
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel(name_));
	model_->SetIsActive(true);

	transform.translate = offset_;
	transform.UpdateMatrix();
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName(name_);
	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Boss);
	collider_->SetCollisionMask(~CollisionAttribute::Boss);
	collider_->SetIsActive(true);
#pragma endregion
}

void BossModel::Update() {
	DrawImGui();
	UpdateTransform();
}

void BossModel::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	//collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void BossModel::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Boss")) {
		if (ImGui::TreeNode(name_.c_str())) {
			ImGui::DragFloat3("scale", &transform.scale.x, 0.1f);
			rotate_ *= Math::ToDegree;
			ImGui::DragFloat3("rotate", &rotate_.x, 0.1f);
			rotate_ *= Math::ToRadian;
			ImGui::DragFloat3("pos", &transform.translate.x, 0.1f);
			ImGui::DragFloat3("offset", &offset_.x, 0.1f);
			ImGui::Text("worldPos x:%f,y:%f,z:%f", transform.worldMatrix.GetTranslate().x, transform.worldMatrix.GetTranslate().y, transform.worldMatrix.GetTranslate().z, 0.1f);
			if (ImGui::Button("Save")) {
				JSON_OPEN("Resources/Data/Boss/Boss.json");
				JSON_OBJECT(name_);
				JSON_SAVE(offset_);
				JSON_CLOSE();
			}
			ImGui::TreePop();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
}

void Body::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void RightArm::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void LeftArm::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void FloorAll::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void LongDistanceAttack::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}
