#pragma once

#include <cstdint>
#include <array>
#include "Graphics/Model.h"
#include "Math/Transform.h"
#include "Math/Random.h"
class Boss;
class BossDustParticle {
public:
	static const uint32_t kParticleNum = 50;

	void Initialize(const Boss* boss);
	void Update();

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

	const Boss* boss_;
	Transform emitTransform_;
	std::array<DustParticle, kParticleNum> particles_;
	float speed_ = 0.1f;
	float scaleSpeed_ = 0.03f;
	Vector3 initialScale = {0.5f,0.5f,0.5f};
	bool isEmit_ = false;
	uint32_t emitNum_ = 1;
	Vector3 minDirection_;
	Vector3 maxDirection_;
	
};