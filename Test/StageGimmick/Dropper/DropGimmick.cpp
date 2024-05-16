#include "DropGimmick.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Player/Player.h"

void DropperBall::Initialize(const Vector3& pos) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = Vector3::one;
	transform.rotate = Quaternion::identity;
	transform.translate = pos;

	model_->SetModel(ResourceManager::GetInstance()->FindModel("dropBall"));
	model_->SetIsActive(true);

	state_ = kDrop;
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("DropGimmickBall");
	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = model_->GetModel()->GetMeshes().at(0).minVertex - model_->GetModel()->GetMeshes().at(0).maxVertex;
	collider_->SetSize({ transform.scale.x* modelSize .x,transform.scale.y * modelSize.y,transform.scale.z * modelSize.z});
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::DropGimmickDropperBall);
	collider_->SetCollisionMask(~CollisionAttribute::DropGimmickDropperBall);
	collider_->SetIsActive(true);
#pragma endregion
	UpdateTransform();
}

void DropperBall::Update() {
	switch (state_) {
	case DropperBall::kDrop:
		transform.translate.y -= 0.5f;
		break;
	case DropperBall::kShot:
		transform.translate.z += 1.0f;
		break;
	}
	UpdateTransform();
}

void DropperBall::UpdateTransform() {
	transform.UpdateMatrix(); 
	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = model_->GetModel()->GetMeshes().at(0).minVertex - model_->GetModel()->GetMeshes().at(0).maxVertex;
	collider_->SetSize({ transform.scale.x * modelSize.x,transform.scale.y * modelSize.y,transform.scale.z * modelSize.z });
	model_->SetWorldMatrix(transform.worldMatrix);
}

void DropperBall::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Player") {
		if (Character::currentCharacterState_ == Character::State::kChase &&
			state_ == State::kStay) {
			state_ = State::kShot;
		}
	}
	else if (collisionInfo.collider->GetName() == "Boss") {
		if (Character::currentCharacterState_ == Character::State::kChase &&
			state_ == State::kShot) {
			isAlive_ = false;
		}
	}
	else if (collisionInfo.collider->GetName() == "Block" ||
		collisionInfo.collider->GetName() == "FireBarCenter" ||
		collisionInfo.collider->GetName() == "Floor" ||
		collisionInfo.collider->GetName() == "StageObject") {
		if (state_ == State::kDrop) {
			state_ = State::kStay;
		}
	}
}

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
	camera_;
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
	camera_;
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
	isCreate_ = false;
}

void DropGimmick::Update() {
	bool isAllSwich = true;
	for (auto& button: switch_) {
		button->Update();
		if (!button->GetIsPushed()) {
			isAllSwich = false;
		}
	}
	for (auto& ball : dropperBall_) {
		ball->Update();
	}
	for (auto& dropper:dropper_) {
		dropper->Update();
	}
	if (!isCreate_ && isAllSwich) {
		isCreate_ = true;
		for (auto& dropper : dropper_) {
			DropperBall* ball = new DropperBall();
			ball->SetPlayer(player_);
			ball->SetCamera(camera_);
			ball->Initialize(dropper->transform.worldMatrix.GetTranslate());
			dropperBall_.emplace_back(std::move(ball));
		}
	}
}
