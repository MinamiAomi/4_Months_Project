#include "Pendulum.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

const std::string Stick::kModelName = "stick";
const std::string Ball::kModelName = "ball";

void Stick::Initialize(const Transform* Transform, float length, float scale) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));
	model_->SetIsActive(true);
	length;
	transform.SetParent(Transform);
	transform.rotate = Quaternion::identity;
	transform.translate = Vector3::zero;
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	transform.scale = { scale , length / modelSize.y,scale };
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("PendulumStick");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
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

void Stick::SetDesc(float length, float scale) {
	transform.rotate = Quaternion::identity;
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	transform.translate = Vector3::zero;
	transform.scale = { scale , length / modelSize.y,scale };
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

void Ball::Initialize(const Transform* Transform, float length, float scale) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));
	model_->SetIsActive(true);

	transform.SetParent(Transform);
	transform.rotate = Quaternion::identity;
	transform.translate = Vector3::zero;
	transform.translate.y = -length;
	transform.scale = { scale,scale,scale };
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

void Ball::SetDesc(float length, float scale) {
	transform.rotate = Quaternion::identity;
	transform.translate = Vector3::zero;
	transform.translate.y = -length;
	transform.scale = { scale,scale,scale };
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
	desc_ = desc;


	transform.translate = desc_.desc.transform.translate;
	transform.rotate = desc_.desc.transform.rotate;
	transform.scale = desc_.desc.transform.scale;


	transform.rotate = Quaternion::MakeForZAxis(desc_.initializeAngle);
	stick_ = std::make_unique<Stick>();
	ball_ = std::make_unique<Ball>();

	angularVelocity_ = 0.0f;
	angularAcceleration_ = 0.0f;
	angularVelocity_ = 0.0f;
	float angle = desc_.angle;
	// 速度計算
	if (angle != desc_.initializeAngle) {
		if (angle > 0) {
			while (angle >= desc_.initializeAngle) {
				angularAcceleration_ = -(desc_.gravity / desc_.length) * std::sin(angle);
				angularVelocity_ += angularAcceleration_;
				angle += angularVelocity_;
			}
		}
		else {
			while (angle <= desc_.initializeAngle) {
				angularAcceleration_ = -(desc_.gravity / desc_.length) * std::sin(angle);
				angularVelocity_ += angularAcceleration_;
				angle += angularVelocity_;
			}
		}
	}
	angle_ = desc_.initializeAngle;
	UpdateTransform();
	stick_->Initialize(&transform, desc_.length, desc_.stickScale);
	ball_->Initialize(&transform, desc_.length, desc_.ballScale);
}

void Pendulum::Update() {
	if (std::fabs((camera_->GetPosition() - transform.worldMatrix.GetTranslate()).Length()) <= 100.0f) {
		angularAcceleration_ = -(desc_.gravity / desc_.length) * std::sin(angle_);

		angularVelocity_ += angularAcceleration_;
		angle_ += angularVelocity_;
		UpdateTransform();

		ball_->Update();
		stick_->Update();
		// 雑カリング
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
	desc_.gravity = 0.002f;

	transform.translate = desc_.desc.transform.translate;
	transform.rotate = desc_.desc.transform.rotate;
	transform.scale = desc_.desc.transform.scale;


	transform.rotate = Quaternion::MakeForZAxis(desc_.initializeAngle);

	angularAcceleration_ = 0.0f;
	float angle = desc_.angle;
	// 速度計算
	if (std::fabsf(angle) > std::fabsf(desc_.initializeAngle) &&
		desc_.gravity != 0.0f) {
		if (angle > 0) {
			while (angle >= desc_.initializeAngle) {
				angularAcceleration_ = -(desc_.gravity / desc_.length) * std::sin(angle);
				angularVelocity_ += angularAcceleration_;
				angle += angularVelocity_;
			}
		}
		else {
			while (angle <= desc_.initializeAngle) {
				angularAcceleration_ = -(desc_.gravity / desc_.length) * std::sin(angle);
				angularVelocity_ += angularAcceleration_;
				angle += angularVelocity_;
			}
		}
	}
	angle_ = desc_.initializeAngle;

	UpdateTransform();
	ball_->SetDesc(desc_.length, desc_.ballScale);
	stick_->SetDesc(desc_.length, desc_.stickScale);
}

void Pendulum::UpdateTransform() {
	transform.rotate = Quaternion::MakeForZAxis(angle_);
	transform.UpdateMatrix();
}