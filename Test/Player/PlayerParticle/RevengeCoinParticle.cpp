#include "RevengeCoinParticle.h"
#include "Player/Player.h"
#include "Framework/ResourceManager.h"

void RevengeCoinParticle::Initialize() {
	minDirection_ = { -0.5f,0.5f,-0.5f };
	maxDirection_ = { 0.5f,0.8f,0.5f };
	emitTransform_.SetParent(&player_->transform,false);
	for (Particle& particle : particles_) {
		particle.model_ = std::make_unique<ModelInstance>();
		particle.model_->SetModel(ResourceManager::GetInstance()->FindModel("box"));
		particle.model_->SetColor({0.005f,0.6f,0.95f});
	}
	Reset();
}

void RevengeCoinParticle::Update() {
	Emit();
	ParticleUpdate();
}

void RevengeCoinParticle::Reset() {
	for (Particle & particle : particles_) {
		particle.isActive = false;
		particle.model_->SetIsActive(false);
	}
}

void RevengeCoinParticle::Emit()
{

	if (isEmit_) {

		emitTransform_.UpdateMatrix();

		for (uint32_t i = 0; i < emitNum_; i++) {
			for (Particle& particle : particles_) {
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
		isEmit_ = false;
	}
}

void RevengeCoinParticle::ParticleUpdate()
{
	for (uint32_t i = 0; Particle & particle : particles_) {
		float rotationSpeed = (2.0f * Math::ToRadian) * (float(i % 2) * 2.0f - 1.0f);
		if (particle.isActive) {
			particle.transform.translate += particle.direction * speed_;
			particle.transform.rotate *= Quaternion::MakeFromAngleAxis(rotationSpeed, particle.direction);
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
