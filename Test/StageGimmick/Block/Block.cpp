#include "Block.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"


void Block::Initialize(const StageGimmick::Desc& desc) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = desc.transform.scale;
	rotate_ = desc.transform.rotate;
	transform.translate = desc.transform.translate;

	colliderDesc_ = desc.collider;

	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);

	model_->SetModel(ResourceManager::GetInstance()->FindModel(desc.name));
	model_->SetIsActive(true);

	onPlayer_ = false;
	onceOnPlayer_ = false;

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Block");
	collider_->SetCenter(colliderDesc_.center * transform.worldMatrix);
	collider_->SetOrientation(transform.rotate * Quaternion::MakeFromEulerAngle(colliderDesc_.rotate));
	collider_->SetSize(colliderDesc_.size);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Block);
	collider_->SetCollisionMask(~CollisionAttribute::Block);
	collider_->SetIsActive(true);
#pragma endregion
}

void Block::Update() {
	// 一回でもブロックに乗っていて今プレイヤーがブロックに乗っていなかったら
	if (!onPlayer_ &&
		onceOnPlayer_) {
		transform.translate.y -= 0.05f;
	}
	transform.translate.y = (std::max)(transform.translate.y, (-transform.scale.y * 0.5f) - 3.0f);
	// 雑カリング
	if (std::fabs((camera_->GetPosition()- transform.worldMatrix.GetTranslate()).Length()) <= 200.0f) {
		model_->SetIsActive(true);
		collider_->SetIsActive(true);
	}
	else {
		model_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
	UpdateTransform();
	onPlayer_ = false;
}

void Block::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.UpdateMatrix();
	collider_->SetCenter(colliderDesc_.center);
	collider_->SetOrientation(Quaternion::MakeFromEulerAngle(colliderDesc_.rotate));	
	collider_->SetSize(colliderDesc_.size);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Block::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Player") {
		// 落下しているとき
		if (Dot(collisionInfo.normal, Vector3::down) >= 0.8f &&
			player_->GetVelocity().y <= 0.0f) {
			onPlayer_ = true;
			onceOnPlayer_ = true;
		}
	}
}
