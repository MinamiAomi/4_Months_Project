#include "PlayerHammerParticle.h"
#include "Player/Player.h"
#include "Framework/ResourceManager.h"
#include "PlayerHammer.h"

void PlayerHammerParticle::Initialize(PlayerHammer& playerHammer) {
	minDirection_ = { -0.5f,0.5f,-0.5f };
	maxDirection_ = { 0.5f,0.8f,0.5f };
	emitTransform_.SetParent(&playerHammer.transform,false);
	emitTransform_.translate.y = 2.0f;
	playerHammer_ = &playerHammer;
	for (DustParticle& particle : particles_) {
		particle.model_ = std::make_unique<ModelInstance>();
		particle.model_->SetModel(ResourceManager::GetInstance()->FindModel("box"));
		particle.model_->SetColor({1.0f,1.0f,0.0f});
	}
	emitFrame_ = -1;
	Reset();
}

void PlayerHammerParticle::Update() {
	Emit();
	ParticleUpdate();
}

void PlayerHammerParticle::Reset() {
	for (DustParticle & particle : particles_) {
		particle.isActive = false;
		particle.model_->SetIsActive(false);
	}
}

void PlayerHammerParticle::Emit()
{
	

	if (emitFrame_ > 0) {
		emitFrame_--;
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
	else if (emitFrame_ == 0) {
		emitFrame_--;
		playerHammer_->GetModel().SetIsActive(isAppear_);
	}
}

void PlayerHammerParticle::ParticleUpdate()
{
	for (uint32_t i = 0; DustParticle & particle : particles_) {
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
