#include "Wind.h"

#include "Framework/ResourceManager.h"

void Wind::Initialize(const Desc& desc) {
	desc_ = desc;
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("wind"));
	model_->SetIsActive(true);
	transform.translate = desc_.position;
	transform.rotate = Quaternion::identity;
	transform.scale = desc_.scale.start;
	isAlive_ = true;
	lifeTime_ = desc_.lifeTime;
	UpdateTransform();
}

void Wind::Update() {
	if (lifeTime_ > 0) {
		transform.translate += desc_.velocity;
		rotate_ += desc_.rotate;
		transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
		float t = 1.0f - (float(lifeTime_) / float(desc_.lifeTime));
		transform.scale = Vector3::Lerp(t, desc_.scale.start, desc_.scale.end);
		lifeTime_--;
	}
	else {
		isAlive_ = false;
	}
	UpdateTransform();
}

void Wind::UpdateTransform() {
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}
