#include "DropGimmick.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Player/Player.h"
#include "Boss/Boss.h"

void DropperBall::Initialize(const Vector3& pos) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = Vector3::one;
	transform.rotate = Quaternion::identity;
	transform.translate = pos;

	model_->SetModel(ResourceManager::GetInstance()->FindModel("dropBall"));
	model_->SetIsActive(true);

	state_ = kDrop;

	random_.x = rnd_.NextFloatRange(-30.0f, 30.0f);
	random_.z = rnd_.NextFloatRange(-5.0f, 5.0f);
	random_.y = rnd_.NextFloatRange(30.0f, 40.0f);
	isAlive_ = true;
	time_ = 0.0f;
	setPos_ = Vector3::zero;
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("DropGimmickBall");
	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = model_->GetModel()->GetMeshes().at(0).minVertex - model_->GetModel()->GetMeshes().at(0).maxVertex;
	collider_->SetSize({ transform.scale.x * modelSize.x,transform.scale.y * modelSize.y,transform.scale.z * modelSize.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::DropGimmickDropperBall);
	collider_->SetCollisionMask(~CollisionAttribute::DropGimmickDropperBall);
	collider_->SetIsActive(true);
#pragma endregion
	UpdateTransform();
}

void DropperBall::Update() {
	camera_;
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	Math::Sphere model{}, camera{};
	model.center = (transform.worldMatrix.GetTranslate());
	model.radius = ((std::max)(modelSize.z * transform.scale.z, std::max(modelSize.x * transform.scale.x, modelSize.y * transform.scale.y)));
	camera.center = (camera_->GetPosition());
	camera.radius = (camera_->GetFarClip());
	if (Math::IsCollision(model, camera)) {
		model_->SetIsActive(true);
		collider_->SetIsActive(true);
		switch (state_) {
		case DropperBall::kDrop:
			transform.translate.y -= 0.5f;
			break;
		case DropperBall::kShot:
			static const float kMax = 60.0f;
			transform.translate = Vector3::QuadraticBezierCurve(
				time_ / kMax,
				setPos_,
				setPos_ + random_ + Vector3(0.0f, 0.0f, boss_->transform.worldMatrix.GetTranslate().z * 0.5f),
				boss_->transform.worldMatrix.GetTranslate());
			time_ += 1.0f;
			if (time_ >= kMax) {
				isAlive_ = false;
			}
			break;
		}
		UpdateTransform();
	}
	else {
		model_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
	
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
			boss_->GetBossHP()->AddBallHitHP();
			isAlive_ = false;
		}
	}
	else if (collisionInfo.collider->GetName() == "Block" ||
		collisionInfo.collider->GetName() == "FireBarCenter" ||
		collisionInfo.collider->GetName() == "Floor" ||
		collisionInfo.collider->GetName() == "StageObject") {
		if (state_ == State::kDrop) {
			state_ = State::kStay;
			setPos_ = transform.translate;
		}
	}
}

void Switch::Initialize(const Desc& desc) {
	model_ = std::make_unique<ModelInstance>();

	desc_ = desc;

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
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	Math::Sphere model{}, camera{};
	model.center = (transform.worldMatrix.GetTranslate());
	model.radius = ((std::max)(modelSize.z * transform.scale.z, std::max(modelSize.x * transform.scale.x, modelSize.y * transform.scale.y)));
	camera.center = (camera_->GetPosition());
	camera.radius = (camera_->GetFarClip());
	if (Math::IsCollision(model, camera)) {
		transform.UpdateMatrix();
		model_->SetIsActive(true);
		collider_->SetIsActive(true);
		UpdateTransform();
	}
	else {
		model_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
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
		if (Character::currentCharacterState_ == Character::State::kRunAway) {
			// 落下しているとき
			if (Dot(collisionInfo.normal, Vector3::down) >= 0.8f &&
				player_->GetVelocity().y <= 0.0f) {
				isPushed_ = true;
			}
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
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	Math::Sphere model{}, camera{};
	model.center = (transform.worldMatrix.GetTranslate());
	model.radius = ((std::max)(modelSize.z * transform.scale.z, std::max(modelSize.x * transform.scale.x, modelSize.y * transform.scale.y)));
	camera.center = (camera_->GetPosition());
	camera.radius = (camera_->GetFarClip());
	if (Math::IsCollision(model, camera)) {
		transform.UpdateMatrix();
		model_->SetIsActive(true);
		collider_->SetIsActive(true);
		UpdateTransform();
	}
	else {
		model_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
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
		dropper->SetCamera(camera_);
		dropper->SetPlayer(player_);
		dropper->SetBoss(boss_);
		dropper->Initialize(dropperDesc);
		dropper_.emplace_back(std::move(dropper));
	}
	for (auto& switchDesc : desc.switchDesc) {
		Switch* button = new Switch();
		button->SetCamera(camera_);
		button->SetPlayer(player_);
		button->SetBoss(boss_);
		button->Initialize(switchDesc);
		switch_.emplace_back(std::move(button));
	}
	isCreate_ = false;
}

void DropGimmick::Update() {
	bool isAllSwich = true;
	for (auto& button : switch_) {
		button->Update();
		if (!button->GetIsPushed()) {
			isAllSwich = false;
		}
	}
	for (auto it = dropperBall_.begin(); it != dropperBall_.end(); ) {
		if (!(*it)->GetIsAlive()) {
			it = dropperBall_.erase(it); // eraseは次のイテレータを返す
		}
		else {
			(*it)->Update();
			++it;
		}
	}
	for (auto& dropper : dropper_) {
		dropper->Update();
	}
	if (!isCreate_ && isAllSwich) {
		isCreate_ = true;
		for (auto& dropper : dropper_) {
			DropperBall* ball = new DropperBall();
			ball->SetPlayer(player_);
			ball->SetCamera(camera_);
			ball->SetBoss(boss_);
			ball->Initialize(dropper->transform.worldMatrix.GetTranslate());
			dropperBall_.emplace_back(std::move(ball));
		}
	}
}
