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

	ModelInstance* GetModel() { return model_.get(); }
private:
	void UpdateTransform();

	std::unique_ptr<ModelInstance> model_;
};