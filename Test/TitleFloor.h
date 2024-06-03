#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Graphics/Model.h"

class TitleFloor :
	public GameObject {
public:
	void Initialize();
	void Update();
private:
	void UpdateTransform();

	std::unique_ptr<ModelInstance> model_;
};