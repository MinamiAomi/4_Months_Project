#include "Pendulum.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

const std::string Pendulum::kStickName = "block";
const std::string Pendulum::kBallName = "block";
void Pendulum::Initialize(const Desc& desc) {

	stick_ = std::make_unique<ModelInstance>();
	ball_ = std::make_unique<ModelInstance>();

	transform.scale = desc.scale;
	rotate_ = desc.rotate;
	pos_ = desc.translate;
	transform.rotate = Quaternion::MakeFromEulerAngle(Vector3{ 0.0f, 0.0f ,270.0f * Math::ToRadian } - rotate_);
	transform.rotate = Quaternion::MakeForZAxis(Math::ToRadian * 1.0f);
	transform.translate = (transform.rotate.GetForward() * transform.scale.z * 0.5f) + pos_;

	ballTransform_.scale = desc.ballScale;
	ballRotate_ = desc.ballRotate;
	transform.rotate = Quaternion::MakeFromEulerAngle(ballRotate_);
	ballTransform_.translate = transform.rotate.GetForward() * (transform.scale.z + ballTransform_.scale.z);

	stick_->SetModel(ResourceManager::GetInstance()->FindModel(kStickName));
	stick_->SetIsActive(true);
	ball_->SetModel(ResourceManager::GetInstance()->FindModel(kStickName));
	ball_->SetIsActive(true);

	speed_ = desc.speed;
	length_ = desc.length;
	angle_ = desc.angle;

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("PendulumBall");
	collider_->SetCenter(ballTransform_.translate);
	collider_->SetOrientation(ballTransform_.rotate);
	collider_->SetSize(ballTransform_.scale);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::PendulumBall);
	collider_->SetCollisionMask(~CollisionAttribute::PendulumBall);
	collider_->SetIsActive(true);
#pragma endregion
}

void Pendulum::Update() {
	static bool  clockwise = true;
	if (clockwise) {
		rotate_.z += speed_;
	}
	else {
		rotate_.z -= speed_;
	}
	if (std::fabs(rotate_.z) >= angle_ * Math::ToRadian) {
		clockwise ^= true;
	}
	UpdateTransform();
}

void Pendulum::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(Vector3{ 0.0f, 0.0f,270.0f * Math::ToRadian } - rotate_);
	transform.translate = (transform.rotate.GetForward() * transform.scale.x) + pos_;
	transform.UpdateMatrix();
	stick_->SetWorldMatrix(transform.worldMatrix);

	ballTransform_.rotate = Quaternion::MakeFromEulerAngle(ballRotate_);
	ballTransform_.translate = transform.rotate.GetForward() * (transform.scale.x + ballTransform_.scale.x);

	ballTransform_.UpdateMatrix();
	ball_->SetWorldMatrix(ballTransform_.worldMatrix);
	Vector3 scale, translate;
	Quaternion rotate;
	ballTransform_.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	collider_->SetSize(scale);
	collider_->SetOrientation(rotate);
}

void Pendulum::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

void PendulumDesc::PendulumDesc::Update() {
	angularAcceleration = -(gravity / length) * std::sin(angle);

	angularVelocity += angularAcceleration;
	angle += angularVelocity;
};

const Vector3 PendulumDesc::PendulumDesc::GetPosition() {
	return { anchor.x + std::sin(angle) * length,anchor.y - std::cos(angle) * length,anchor.z };
}
