#include "BossBullet.h"

#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"


void BossBullet::Initialize(const Transform& parent,const Vector3& position, const Vector3& velocity) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("bossBullet"));
	model_->SetIsActive(true);
	lifeTime_ = 0;
	transform.SetParent(&parent);
	transform.translate = position;
	transform.rotate = Quaternion::identity;
	transform.scale = Vector3::one;
	velocity_ = velocity;
	isAlive_ = true;
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("bossBullet");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x,modelSize.y ,modelSize.z });
	collider_->SetSize(transform.scale);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Boss);
	collider_->SetCollisionMask(CollisionAttribute::Player | CollisionAttribute::Ground);
	collider_->SetIsActive(true);
#pragma endregion
	UpdateTransform();
}



void BossBullet::Update() {
	if (lifeTime_>=600) {
		isAlive_ = false;
	}
	else {
		lifeTime_++;
	}
	transform.translate += velocity_;
	UpdateTransform();

}

void BossBullet::UpdateTransform() {
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

void BossBullet::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Block" ||
		collisionInfo.collider->GetName() == "FireBarCenter" ||
		collisionInfo.collider->GetName() == "Floor" ||
		collisionInfo.collider->GetName() == "StageObject" ||
		collisionInfo.collider->GetName() == "BeltConveyor" ||
		collisionInfo.collider->GetName() == "DropGimmickDropper" ||
		collisionInfo.collider->GetName() == "DropGimmickSwitch" ||
		collisionInfo.collider->GetName() == "Player") {
		isAlive_ = false;
	}
}
