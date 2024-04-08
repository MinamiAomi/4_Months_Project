#include "Block.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

const std::string Block::kModelName = "block";

void Block::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos) {
	model_ = std::make_unique<ModelInstance>();

	transform.scale = scale;
	rotate_ = rotate;
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.translate = pos;

	model_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));
	model_->SetIsActive(true);

	onPlayer_ = false;
	onceOnPlayer_ = false;

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Block");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	collider_->SetSize(transform.scale);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Block);
	collider_->SetCollisionMask(~CollisionAttribute::Block);
	collider_->SetIsActive(true);
#pragma endregion
}

void Block::Update() {
	// 一回でもブロックに乗っていて今プレイヤーがブロックに乗っていなかったら
	if (!onPlayer_ &&
		onceOnPlayer_ &&
		transform.translate.y > -transform.scale.y * 0.5f) {
		transform.translate.y -= 0.05f;
	}
	transform.translate.y = (std::max)(transform.translate.y, (-transform.scale.y * 0.5f) - 0.5f);
	UpdateTransform();
	onPlayer_ = false;
}

void Block::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	collider_->SetSize(scale);
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Block::OnCollision(const CollisionInfo& collisionInfo) {
	if (collisionInfo.collider->GetName() == "Player") {
		if (std::fabs(Dot(collisionInfo.normal, Vector3::down)) >= 0.5f) {
			onPlayer_ = true;
			onceOnPlayer_ = true;
		}
	}
}
