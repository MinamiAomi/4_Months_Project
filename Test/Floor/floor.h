#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <string>

#include "Math/MathUtils.h"
#include "Graphics/Model.h"

class Floor :
	public GameObject {
public:
	void Initialize();
	void Update();

	const Vector3& GetLocalPos() const { return transform.translate; }
	void SetLocalPos(const Vector3& pos ) { transform.translate = pos; }
	const Matrix4x4& GetWorld() const { return transform.worldMatrix; }
	const float GetZLength() const { return zLength_; };
public:
	
private:
	std::unique_ptr<ModelInstance> model_;
	float zLength_ = 0.0f;
};