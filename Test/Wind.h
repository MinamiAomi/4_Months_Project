#pragma once

#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Graphics/Model.h"

class Wind :
	public GameObject {
public:
	struct Desc {
		struct Scale {
			Vector3 start;
			Vector3 end;
		}scale;
		Vector3 position;
		Vector3 velocity;
		Vector3 rotate;
		uint32_t lifeTime;
	};

	void Initialize(const Desc& desc);
	void Update();


	bool GetIsAlive() { return isAlive_; }
	void SetIsAlive(bool flag) { isAlive_ = flag; }
private:
	void UpdateTransform();
	std::unique_ptr<ModelInstance> model_;

	Desc desc_;

	Vector3 rotate_;

	uint32_t lifeTime_;

	bool isAlive_;
};