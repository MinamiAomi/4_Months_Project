#include "BeltConveyor.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void BeltConveyor::Initialize(const Desc& desc) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = desc.desc.transform.scale;
	transform.rotate = desc.desc.transform.rotate;
	transform.translate = desc.desc.transform.translate;

	desc_ = desc;

	model_->SetModel(ResourceManager::GetInstance()->FindModel(desc.desc.name));
	model_->SetIsActive(true);

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("BeltConveyor");
	collider_->SetCenter(desc_.desc.collider->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * desc_.desc.collider->rotate);
	collider_->SetSize({ transform.scale.x * desc_.desc.collider->size.x ,transform.scale.y * desc_.desc.collider->size.y ,transform.scale.z * desc_.desc.collider->size.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Block);
	collider_->SetCollisionMask(~CollisionAttribute::Block);
	collider_->SetIsActive(true);
#pragma endregion
	UpdateTransform();
}

void BeltConveyor::Update() {
	if (std::fabs((camera_->GetPosition() - transform.worldMatrix.GetTranslate()).Length()) <= 200.0f) {
		transform.translate.y = (std::max)(transform.translate.y, (-transform.scale.y * 0.5f) - 3.0f);
		UpdateTransform();
		// 雑カリング
		model_->SetIsActive(true);
		collider_->SetIsActive(true);
	}
	else {
		model_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
}

void BeltConveyor::UpdateTransform() {
	transform.UpdateMatrix();
	collider_->SetCenter(desc_.desc.collider->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * desc_.desc.collider->rotate);
	collider_->SetSize({ transform.scale.x * desc_.desc.collider->size.x ,transform.scale.y * desc_.desc.collider->size.y ,transform.scale.z * desc_.desc.collider->size.z });
	Matrix4x4 modelMatrix = Matrix4x4::MakeAffineTransform(transform.scale, transform.rotate* Quaternion::MakeForYAxis(90.0f * Math::ToRadian),transform.translate);
	model_->SetWorldMatrix(modelMatrix);
}

void BeltConveyor::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Player") {
		// 落下しているとき
		if (Dot(collisionInfo.normal, Vector3::down) >= 0.8f &&
			player_->GetVelocity().y <= 0.0f) {
			player_->SetBeltConveyorVelocity(desc_.velocity);
		}
	}
}
