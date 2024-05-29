#pragma once

#include <cstdint>
#include <array>
#include "Graphics/Model.h"
#include "Math/Transform.h"
#include "Math/Random.h"

class PlayerHammer;
class PlayerHammerParticle {
public:
	static const uint32_t kParticleNum = 50;

	void Initialize(PlayerHammer& playerHammer);
	void Update();
	void SetFrame(int frame,bool is) { 
		emitFrame_ = frame;
		isAppear_ = is;
	}

	void Reset();

	struct DustParticle {
		Transform transform;
		Vector3 direction;
		Vector3 velocity;
		std::unique_ptr<ModelInstance> model_;
		bool isActive;
	};

private:

	void Emit();
	void ParticleUpdate();

	Random::RandomNumberGenerator rng_;

	PlayerHammer* playerHammer_;

	Transform emitTransform_;
	
	std::array<DustParticle, kParticleNum> particles_;
	float speed_ = 0.1f;
	float scaleSpeed_ = 0.08f;
	Vector3 initialScale = {6.0f,6.0f,6.0f};
	int emitFrame_ = -1;
	uint32_t emitNum_ = 3;
	Vector3 minDirection_;
	Vector3 maxDirection_;
	
	bool isAppear_ = false;
};