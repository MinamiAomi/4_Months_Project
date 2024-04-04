#pragma once
#include <memory>
#include <string>

#include "Collision/GameObject.h"
#include "Graphics/Model.h"
#include "Engine/Math/MathUtils.h"

class Block :
	public GameObject {
public:
	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos);
	void Update();

	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { rotate_= rotate; }
	void SetPosition(const Vector3& pos) { transform.translate = pos; }
	const Vector3& GetScale() { return transform.scale; }
	const Vector3& GetRotate() { return rotate_; }
	const Vector3& GetPosition() { return transform.translate; }
private:
	static const std::string kModelName;

	std::unique_ptr<ModelInstance> model_;

	Vector3 rotate_;
};