#include "Pendulum.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

const std::string Stick::kModelName = "block";
const std::string Ball::kModelName = "block";

void Stick::Initialize(const Transform* Transform, const Desc& desc) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));
	model_->SetIsActive(true);

	transform.SetParent(Transform);
	transform.scale = desc.scale;
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("PendulumStick");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::PendulumBall);
	collider_->SetCollisionMask(~CollisionAttribute::PendulumBall);
	collider_->SetIsActive(true);
#pragma endregion
}

void Stick::SetDesc(const Desc& desc) {
	transform.scale = desc.scale;
	UpdateTransform();
}

void Stick::Update(float angle) {
	transform.rotate = Quaternion::MakeForZAxis(angle);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	// z軸で回転した後のoffsetを取得
	Vector3 offset = Quaternion::MakeForZAxis(angle).GetForward();

	// y軸方向に移動するための変換
	offset = Vector3(offset.x, offset.y, 0.0f);

	// y軸方向に移動
	transform.translate = offset * (modelSize.y * transform.scale.y);

	UpdateTransform();
}

void Stick::UpdateTransform() {

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

void Stick::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void Ball::Initialize(const Transform* Transform, const Desc& desc) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));
	model_->SetIsActive(true);

	transform.SetParent(Transform);
	transform.scale = desc.scale;
	length_ = desc.length;
	gravity_ = desc.gravity;
	angle_ = desc.angle;
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("PendulumBall");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::PendulumBall);
	collider_->SetCollisionMask(~CollisionAttribute::PendulumBall);
	collider_->SetIsActive(true);
#pragma endregion
}

void Ball::Update() {
	angularAcceleration_ = -(gravity_ / length_) * std::sin(angle_);

	angularVelocity_ += angularAcceleration_;
	angle_ += angularVelocity_;

	UpdateTransform();
}

void Ball::SetDesc(const Desc& desc) {
	transform.scale = desc.scale;
	length_ = desc.length;
	gravity_ = desc.gravity;
	angle_ = desc.angle;
	UpdateTransform();
}

void Ball::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.translate = { std::sin(angle_) * length_, -std::cos(angle_) * length_, 0.0f };
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

void Ball::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void Pendulum::Initialize(const Desc& desc) {
	stick_ = std::make_unique<Stick>();
	ball_ = std::make_unique<Ball>();

	transform.translate = desc.pos;

	desc_ = desc;

	stick_->Initialize(&transform, desc.stickDesc);
	ball_->Initialize(&transform, desc.ballDesc);
}

void Pendulum::Update() {
	transform.translate = desc_.pos;
	ball_->Update();
	stick_->Update(ball_->GetAngle());
	UpdateTransform();
}

void Pendulum::SetDesc(const Desc& desc) {
	desc_ = desc;
	transform.translate = desc.pos;
	UpdateTransform();
	ball_->SetDesc(desc_.ballDesc);
	stick_->Update(ball_->GetAngle());
	stick_->SetDesc(desc_.stickDesc);
}

void Pendulum::UpdateTransform() {
	transform.UpdateMatrix();
}