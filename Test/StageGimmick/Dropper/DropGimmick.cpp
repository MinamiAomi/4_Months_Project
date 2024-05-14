#include "DropGimmick.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Player/Player.h"

void Switch::Initialize(const Desc& desc) {
	model_ = std::make_unique<ModelInstance>();
	
	desc_= desc;

	transform.scale = desc.desc.transform.scale;
	transform.rotate = desc.desc.transform.rotate;
	transform.translate = desc.desc.transform.translate;

	model_->SetModel(ResourceManager::GetInstance()->FindModel(desc.desc.name));
	model_->SetIsActive(true);
	
	isPushed_ = false;
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("DropGimmickSwitch");
	collider_->SetCenter(desc_.desc.collider->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * desc_.desc.collider->rotate);
	collider_->SetSize({ transform.scale.x * desc_.desc.collider->size.x ,transform.scale.y * desc_.desc.collider->size.y ,transform.scale.z * desc_.desc.collider->size.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::DropGimmickSwitch);
	collider_->SetCollisionMask(~CollisionAttribute::DropGimmickSwitch);
	collider_->SetIsActive(true);
#pragma endregion
}

void Switch::Update() {

	UpdateTransform();
}

void Switch::UpdateTransform() {
	transform.UpdateMatrix();
	collider_->SetCenter(desc_.desc.collider->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * desc_.desc.collider->rotate);
	collider_->SetSize({ transform.scale.x * desc_.desc.collider->size.x ,transform.scale.y * desc_.desc.collider->size.y ,transform.scale.z * desc_.desc.collider->size.z });
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Switch::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Player") {
		// 落下しているとき
		if (Dot(collisionInfo.normal, Vector3::down) >= 0.8f &&
			player_->GetVelocity().y <= 0.0f) {
			isPushed_ = true;
		}
	}
}

void Dropper::Initialize(const Desc& desc) {
	model_ = std::make_unique<ModelInstance>();

	desc_ = desc;

	transform.scale = desc.desc.transform.scale;
	transform.rotate = desc.desc.transform.rotate;
	transform.translate = desc.desc.transform.translate;

	model_->SetModel(ResourceManager::GetInstance()->FindModel(desc.desc.name));
	model_->SetIsActive(true);

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("DropGimmickDropper");
	collider_->SetCenter(desc_.desc.collider->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * desc_.desc.collider->rotate);
	collider_->SetSize({ transform.scale.x * desc_.desc.collider->size.x ,transform.scale.y * desc_.desc.collider->size.y ,transform.scale.z * desc_.desc.collider->size.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::DropGimmickDropper);
	collider_->SetCollisionMask(~CollisionAttribute::DropGimmickDropper);
	collider_->SetIsActive(true);
#pragma endregion
}

void Dropper::Update() {
	UpdateTransform();
}

void Dropper::UpdateTransform() {
	transform.UpdateMatrix();
	collider_->SetCenter(desc_.desc.collider->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * desc_.desc.collider->rotate);
	collider_->SetSize({ transform.scale.x * desc_.desc.collider->size.x ,transform.scale.y * desc_.desc.collider->size.y ,transform.scale.z * desc_.desc.collider->size.z });
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Dropper::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}


void DropGimmick::Initialize(const Desc& desc) {
	for (auto& dropperDesc : desc.dropperDesc) {
		Dropper* dropper = new Dropper();
		dropper->SetPlayer(player_);
		dropper->Initialize(dropperDesc);
		dropper_.emplace_back(std::move(dropper));
	}
	for (auto& switchDesc: desc.switchDesc) {
		Switch* button= new Switch();
		button->SetPlayer(player_);
		button->Initialize(switchDesc);
		switch_.emplace_back(std::move(button));
	}
}

void DropGimmick::Update() {
	bool isAllSwich = true;
	for (auto& button: switch_) {
		button->Update();
		if (!button->GetIsPushed()) {
			isAllSwich = false;
		}
	}
	for (auto& dropper:dropper_) {
		dropper->Update();
	}
}
