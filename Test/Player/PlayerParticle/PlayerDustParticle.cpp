#include "PlayerDustParticle.h"
#include "Player/Player.h"
#include "Framework/ResourceManager.h"

void PlayerDustParticle::Initialize() {
	minDirection_ = { -0.5f,0.5f,-0.5f };
	maxDirection_ = { 0.5f,1.0f,0.5f };
	emitTransform_.SetParent(&player_->transform,false);
	Vector3 modelMinSize = player_->GetMinModelSize();
	//emitTransform_.translate.y = modelMinSize.y - 2.0f;
	emitTransform_.translate.z = modelMinSize.z;
	for (DustParticle& particle : particles_) {
		particle.model_ = std::make_unique<ModelInstance>();
		particle.model_->SetModel(ResourceManager::GetInstance()->FindModel("box"));
	}
	Reset();
}

void PlayerDustParticle::Update() {
	Emit();
	ParticleUpdate();
}

void PlayerDustParticle::Reset() {
	for (DustParticle & particle : particles_) {
		particle.isActive = false;
		particle.model_->SetIsActive(false);
	}
}

void PlayerDustParticle::Emit()
{
	isEmit_ = player_->GetIsMove() && player_->GetIsGround();

	if (isEmit_) {

		emitTransform_.UpdateMatrix();

		for (uint32_t i = 0; i < emitNum_; i++) {
			for (DustParticle& particle : particles_) {
				if (!particle.isActive) {
					particle.isActive = true;
					particle.model_->SetIsActive(true);
					
					particle.transform.translate = emitTransform_.worldMatrix.GetTranslate();
					particle.transform.rotate = Quaternion::identity;
					particle.transform.scale = initialScale;
					particle.direction = Vector3{ rng_.NextFloatRange(minDirection_.x,maxDirection_.x),rng_.NextFloatRange(minDirection_.y,maxDirection_.y) ,rng_.NextFloatRange(minDirection_.z,maxDirection_.z) };
					break;
				}
			}
		}
	}
}

void PlayerDustParticle::ParticleUpdate()
{
	for (uint32_t i = 0; DustParticle & particle : particles_) {
		float rotationSpeed = (2.0f * Math::ToRadian) * (float(i % 2) * 2.0f - 1.0f);
		if (particle.isActive) {
			particle.transform.translate += particle.direction * speed_;
			particle.transform.rotate *= Quaternion::MakeFromAngleAxis(rotationSpeed, { 0.0f,0.0f,1.0f });
			particle.transform.scale.x -= scaleSpeed_;
			particle.transform.scale.y -= scaleSpeed_;
			particle.transform.scale.z -= scaleSpeed_;

			particle.transform.UpdateMatrix();
			particle.model_->SetWorldMatrix(particle.transform.worldMatrix);
			if (particle.transform.scale.x <= 0.0f) {
				particle.isActive = false;
				particle.model_->SetIsActive(false);
			}
		}
		i++;
	}
}
