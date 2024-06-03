#include "DropGimmick.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Player/Player.h"
#include "Boss/Boss.h"

void DropperBall::Initialize(const Desc& desc) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = Vector3::one;
	transform.rotate = Quaternion::identity;
	transform.translate = desc.desc.transform.translate;

	model_->SetModel(ResourceManager::GetInstance()->FindModel("dropBall"));
	model_->SetIsActive(true);

	kickSE_ = std::make_unique<AudioSource>();
	hitSE_ = std::make_unique<AudioSource>();
	(*kickSE_) = ResourceManager::GetInstance()->FindSound("kickDropBall");
	(*hitSE_) = ResourceManager::GetInstance()->FindSound("hitDropBall");

	state_ = kDrop;

	random_.x = rnd_.NextFloatRange(-30.0f, 30.0f);
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
	collider_->SetCollisionMask(CollisionAttribute::Player | CollisionAttribute::Boss | CollisionAttribute::Ground);
	collider_->SetIsActive(true);
#pragma endregion
	UpdateTransform();
}

void DropperBall::Update() {
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
				Vector3(0.0f, 0.0f, boss_->transform.worldMatrix.GetTranslate().z + ((setPos_.z - boss_->transform.worldMatrix.GetTranslate().z) * 0.5f)) + random_,
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
			kickSE_->SetVolume(0.5f);
			kickSE_->Play();
			state_ = State::kShot;
		}
	}
	else if (collisionInfo.collider->GetName() == "Boss") {
		if (Character::currentCharacterState_ == Character::State::kChase &&
			state_ == State::kShot) {
			boss_->GetBossHP()->AddBallHitHP();
			isAlive_ = false;
			hitSE_->SetVolume(1.0f);
			hitSE_->Play();
			boss_->SetChangeColorFrame(5);
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
	switchBase_ = std::make_unique<ModelInstance>();
	switch_ = std::make_unique<ModelInstance>();

	desc_ = desc;

	transform.scale = desc.desc.transform.scale;
	transform.rotate = desc.desc.transform.rotate;
	transform.translate = desc.desc.transform.translate;

	switchTransform_.SetParent(&transform);

	switchBase_->SetModel(ResourceManager::GetInstance()->FindModel("switchBase"));
	switch_->SetModel(ResourceManager::GetInstance()->FindModel("switch"));
	switchBase_->SetIsActive(true);
	switch_->SetIsActive(true);

	time_ = 0.0f;
	isPushed_ = false;
	switchSE_ = std::make_unique<AudioSource>();
	(*switchSE_) = ResourceManager::GetInstance()->FindSound("switchSE");

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("DropGimmickSwitch");
	collider_->SetCenter(desc_.desc.collider->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * desc_.desc.collider->rotate);
	collider_->SetSize({ transform.scale.x * desc_.desc.collider->size.x ,transform.scale.y * desc_.desc.collider->size.y ,transform.scale.z * desc_.desc.collider->size.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Ground);
	collider_->SetCollisionMask(CollisionAttribute::Player | CollisionAttribute::DropGimmickDropperBall | CollisionAttribute::BossBullet);
	collider_->SetIsActive(true);
#pragma endregion
	UpdateTransform();
}

void Switch::Update() {
	camera_;
	Vector3 modelSize = (switchBase_->GetModel()->GetMeshes().at(0).maxVertex - switchBase_->GetModel()->GetMeshes().at(0).minVertex);
	Math::Sphere model{}, camera{};
	model.center = (transform.worldMatrix.GetTranslate());
	model.radius = ((std::max)(modelSize.z * transform.scale.z, std::max(modelSize.x * transform.scale.x, modelSize.y * transform.scale.y)));
	camera.center = (camera_->GetPosition());
	camera.radius = (camera_->GetFarClip());
	if (Math::IsCollision(model, camera)) {
		if (isPushed_) {
			time_ += 1.0f / 15.0f;
			switchTransform_.translate.y = std::lerp(0.0f,-1.0f, time_);
			time_ = std::clamp(time_,0.0f,1.0f);
		}
		transform.UpdateMatrix();
		switchBase_->SetIsActive(true);
		collider_->SetIsActive(true);
		UpdateTransform();
	}
	else {
		switchBase_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
}

void Switch::UpdateTransform() {
	transform.UpdateMatrix();
	switchTransform_.UpdateMatrix();
	collider_->SetCenter(desc_.desc.collider->center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * desc_.desc.collider->rotate);
	collider_->SetSize({ transform.scale.x * desc_.desc.collider->size.x ,transform.scale.y * desc_.desc.collider->size.y ,transform.scale.z * desc_.desc.collider->size.z });
	switchBase_->SetWorldMatrix(transform.worldMatrix);
	switch_->SetWorldMatrix(switchTransform_.worldMatrix);
}

void Switch::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Player") {
		if (Character::currentCharacterState_ == Character::State::kRunAway) {
			// 落下しているとき
			if (Dot(collisionInfo.normal, Vector3::down) >= 0.8f &&
				player_->GetVelocity().y <= 0.0f&&
				!isPushed_) {
				isPushed_ = true;
				switchSE_->Play();
				switchSE_->SetVolume(0.5f);
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
	collider_->SetCollisionAttribute(CollisionAttribute::Ground);
	collider_->SetCollisionMask(CollisionAttribute::Player | CollisionAttribute::DropGimmickDropperBall | CollisionAttribute::BossBullet);
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

	for (auto& dropper : dropper_) {
		dropper->Update();
	}
	if (!isCreate_ && isAllSwich) {
		isCreate_ = true;
		for (auto& dropper : dropper_) {
			DropperBall::Desc desc{};
			desc.desc.transform.translate = dropper->transform.worldMatrix.GetTranslate();
			dropperBallManager_->Create(desc, 0);
		}
	}
}

void DropperBallManager::Create(const DropperBall::Desc& desc, uint32_t index) {
	DropperBall* ball = new DropperBall();
	ball->SetPlayer(player_);
	ball->SetCamera(camera_);
	ball->SetBoss(boss_);
	ball->stageGimmickNumber = index;
	ball->Initialize(desc);
	dropperBalls_.emplace_back(std::move(ball));
}

void DropperBallManager::Update() {
	for (auto it = dropperBalls_.begin(); it != dropperBalls_.end(); ) {
		if (!(*it)->GetIsAlive()) {
			it = dropperBalls_.erase(it); // eraseは次のイテレータを返す
		}
		else {
			(*it)->Update();
			++it;
		}
	}

}

void DropperBallManager::Delete(DropperBall* ball) {
	// block がリスト内に存在するかを確認し、存在する場合はそのイテレータを取得する
	auto it = std::find_if(dropperBalls_.begin(), dropperBalls_.end(), [ball](const auto& ptr) {
		return ptr.get() == ball;
		});

	// block が見つかった場合は削除する
	if (it != dropperBalls_.end()) {
		dropperBalls_.erase(it);
	}
}

void DropperBallManager::Reset() {
	dropperBalls_.clear();
}
