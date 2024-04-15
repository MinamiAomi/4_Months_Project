#include "Pendulum.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

const std::string Stick::kModelName = "stick";
const std::string Ball::kModelName = "ball";

void Stick::Initialize(const Transform* Transform, float length) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));
	model_->SetIsActive(true);

	transform.SetParent(Transform);
	transform.translate.y = -length * 0.5f;
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

void Stick::Update() {
	UpdateTransform();
}

void Stick::SetDesc(float length) {
	transform.translate.y = -length * 0.5f;
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

void Ball::Initialize(const Transform* Transform, float length) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));
	model_->SetIsActive(true);

	transform.SetParent(Transform);
	transform.translate.y = length;
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
	UpdateTransform();
}

void Ball::SetDesc(float length) {
	transform.translate.y = length;
	UpdateTransform();
}

void Ball::UpdateTransform() {
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
	transform.translate = desc_.pos;
	transform.rotate = Quaternion::MakeForZAxis(desc.initializeAngle);
	stick_ = std::make_unique<Stick>();
	ball_ = std::make_unique<Ball>();

	desc_ = desc;
	angularAcceleration_ = 0.0f;
	angularVelocity_ = 0.0f;
	angle_ = desc_.initializeAngle;

	stick_->Initialize(&transform, desc_.length);
	ball_->Initialize(&transform, desc_.length);
}

void Pendulum::Update() {
	angularAcceleration_ = -(desc_.gravity / desc_.length) * std::sin(angle_);

	angularVelocity_ += angularAcceleration_;
	angle_ += angularVelocity_;
	UpdateTransform();

	ball_->Update();
	stick_->Update();
	// 雑カリング
	if (std::fabs((player_->transform.worldMatrix.GetTranslate() - transform.worldMatrix.GetTranslate()).Length()) <= 100.0f) {
		ball_->SetIsActive(true);
		stick_->SetIsActive(true);
	}
	else {
		ball_->SetIsActive(false);
		stick_->SetIsActive(false);
	}
}

void Pendulum::SetDesc(const Desc& desc) {
	desc_ = desc;
	transform.translate = desc_.pos;
	transform.rotate.z = desc_.initializeAngle;
	angularAcceleration_ = 0.0f;
	angularVelocity_ = 0.0f;
	angle_ = desc_.initializeAngle;

	UpdateTransform();
	ball_->SetDesc(desc_.length);
	stick_->SetDesc(desc_.length);
}

void Pendulum::UpdateTransform() {
	transform.rotate = Quaternion::MakeForZAxis(angle_);
	transform.UpdateMatrix();
}