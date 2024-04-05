#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Collision/Collider.h"

#include "Graphics/Model.h"

class Boss :
	public GameObject {
public:
	void Initialize();
	void Update();

	void Reset();
private:
	std::unique_ptr<ModelInstance> model_;
	Vector3 velocity_;
};