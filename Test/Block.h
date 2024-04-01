#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Graphics/Model.h"

class Block :
	public GameObject {
public:
	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos);
	void Update();
	void Draw();
private:
	static const std::string kModelName;
	std::unique_ptr<ModelInstance> model_;
};