#include "StageObject.h"

#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Math/Geometry.h"

void StageObject::Initialize(const StageGimmick::Desc& desc) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = desc.transform.scale;
	transform.rotate = desc.transform.rotate;
	transform.translate = desc.transform.translate;
	desc_ = desc;
	colliderDesc_ = desc.collider;

	model_->SetModel(ResourceManager::GetInstance()->FindModel(desc.name));
	model_->SetIsActive(true);

#pragma region コライダー
	if (colliderDesc_) {
		collider_ = std::make_unique<BoxCollider>();
		collider_->SetGameObject(this);
		collider_->SetName("StageObject");
		collider_->SetCenter(colliderDesc_->center * transform.worldMatrix);
		collider_->SetOrientation(transform.rotate * colliderDesc_->rotate);
		collider_->SetSize({ transform.scale.x * colliderDesc_->size.x ,transform.scale.y * colliderDesc_->size.y ,transform.scale.z * colliderDesc_->size.z });
		//collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
		collider_->SetCollisionAttribute(CollisionAttribute::StageObject);
		collider_->SetCollisionMask(~CollisionAttribute::StageObject);
		collider_->SetIsActive(true);
	}
#pragma endregion
}

void StageObject::Update() {
	UpdateTransform();
}

void StageObject::UpdateTransform() {
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	Math::Sphere model{}, camera{};
	model.center = (transform.worldMatrix.GetTranslate());
	model.radius = ((std::max)(modelSize.z * transform.scale.z,std::max(modelSize.x * transform.scale.x, modelSize.y * transform.scale.y)));
	camera.center = (camera_->GetPosition());
	camera.radius = (camera_->GetFarClip());
	if (Math::IsCollision(model,camera)) {
		transform.UpdateMatrix();
		if (colliderDesc_) {
			collider_->SetCenter(colliderDesc_->center * transform.worldMatrix);
			collider_->SetOrientation(transform.rotate * colliderDesc_->rotate);
			collider_->SetSize({ transform.scale.x * colliderDesc_->size.x ,transform.scale.y * colliderDesc_->size.y ,transform.scale.z * colliderDesc_->size.z });
		}
		model_->SetWorldMatrix(Matrix4x4::MakeRotationY(Math::ToRadian * 90.0f) * transform.worldMatrix);
		model_->SetIsActive(true);
		if (colliderDesc_) {
			collider_->SetIsActive(true);
		}
	}
	else {
		model_->SetIsActive(false);
		if (colliderDesc_) {
			collider_->SetIsActive(false);
		}
	}
}

//void StageObject::OnCollision(const CollisionInfo& collisionInfo) {}
