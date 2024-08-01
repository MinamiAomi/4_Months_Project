#pragma once

#include <cstdint>
#include <array>
#include "Graphics/Model.h"
#include "Math/Transform.h"
#include "Math/Random.h"
class Player;
class RevengeCoinParticle {
public:
	static const uint32_t kParticleNum = 10;

	void Initialize();
	void Update();

	void Reset();
	void SetEmit() { isEmit_ = true; }

	void SetPlayer(Player* player) { player_ = player; }

	struct Particle {
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

	Player* player_;
	Transform emitTransform_;
	std::array<Particle, kParticleNum> particles_;
	float speed_ = 0.1f;
	float scaleSpeed_ = 0.06f;
	Vector3 initialScale = {3.0f,3.0f,3.0f};
	bool isEmit_ = false;
	uint32_t emitNum_ = 10;
	Vector3 minDirection_;
	Vector3 maxDirection_;
	
};