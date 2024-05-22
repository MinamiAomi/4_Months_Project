#include "BossModelManager.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "Graphics/RenderManager.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"



namespace BossParts {
	// 実際の定義
	std::array<std::string, Parts::kCount> partsName_ = {
		"boss_2",
		"bossFloorAll",
		"bossLongDistanceAttack",
	};
}

void BossModelManager::Initialize(const Transform* Transform) {
	models_.at(BossParts::Parts::kFloorAll) = std::make_unique<FloorAll>();
	models_.at(BossParts::Parts::kLongDistanceAttack) = std::make_unique<LongDistanceAttack>();
	models_.at(BossParts::Parts::kBossBody) = std::make_unique<BossBody>();


	for (uint32_t i = 0; auto & model : models_) {
		model->transform.SetParent(Transform);
		model->Initialize(i);
		i++;
	}

	std::vector<std::string> partsName = {
		"nitoukin_R",
		"ude_R",
		"tekubi_R",
		"te_R",

	};
	models_.at(BossParts::Parts::kBossBody)->AddAnimation(partsName,"bossLeftHand");
	models_.at(BossParts::Parts::kBossBody)->SetColliderIsCollision(false);
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

void BossModel::AddAnimation(std::vector<std::string> nameList, std::string colliderName) {
#pragma region アニメーション
	Parts parts{};
	parts.animation = ResourceManager::GetInstance()->FindAnimation(name_);
	parts.skeleton = std::make_shared<Skeleton>();
	parts.skeleton->Create(model_->GetModel());
	parts.InitializeCollider(nameList,colliderName);
	parts_.emplace_back(std::move(parts));
#pragma endregion
}

void BossModel::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
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
	collider_->DebugDraw();
#endif // _DEBUG
}
void BossBody::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void FloorAll::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void LongDistanceAttack::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void BossModel::Parts::SetIsCollision(bool flag) {
	for (auto& part : parts) {
		part.second->SetIsActive(flag);
	}
}

void BossModel::Parts::UpdateCollider(const Matrix4x4& worldMat) {
	for (auto& part : parts) {
		auto joint = skeleton->GetJoint(part.first);
		assert(joint.parent.has_value());
		Matrix4x4 worldMatrix = joint.skeletonSpaceMatrix * Matrix4x4::MakeScaling({ -1.0f, 1.0f, -1.0f }) * worldMat;
		Matrix4x4 parentMatrix = skeleton->GetJoint(*joint.parent).skeletonSpaceMatrix * Matrix4x4::MakeScaling({ -1.0f, 1.0f, -1.0f }) * worldMat;

		Vector3 born = (worldMatrix.GetTranslate() - parentMatrix.GetTranslate());
		part.second->SetCenter(parentMatrix.GetTranslate() + born * 0.5f);
		part.second->SetOrientation(Quaternion::MakeLookRotation(born.Normalized()));
		part.second->SetSize({ 5.0f, 5.0f,born.Length()});
		part.second->DebugDraw(Vector4(0.0f,1.0f,0.0f,1.0f));
	}
}

void BossModel::Parts::InitializeCollider(std::vector<std::string> nameList, std::string colliderName) {
	for (auto& string : nameList) {
		parts[string]=std::make_unique<BoxCollider>();
		parts[string]->SetName(colliderName);
		parts[string]->SetCollisionAttribute(CollisionAttribute::Boss);
		parts[string]->SetCollisionMask(~CollisionAttribute::Boss);
		parts[string]->SetIsActive(false);
	}
}

