#pragma once
#include "Collision/GameObject.h"

#include <vector>
#include <memory>

#include "Collision/Collider.h"
#include "Graphics/Model.h"
#include "StageGimmick/StageGimmick.h"

class Switch :
	public GameObject {
public:
	struct Desc {
		StageGimmick::Desc desc;
		uint32_t index;
	};
	void Initialize(const Desc& desc);
	void Update();
private:
	void UpdateTransform();
	void OnCollision(const CollisionInfo& collisionInfo);
	std::unique_ptr<BoxCollider> collider_;
	std::unique_ptr<ModelInstance> model_;
	Desc desc_;
};

class Dropper :
	public GameObject{
public:
	struct Desc {
		StageGimmick::Desc desc;
		uint32_t index;
	};
	void Initialize(const Desc& desc);
	void Update();
private:
	void OnCollision(const CollisionInfo& collisionInfo);
	void UpdateTransform();
	std::unique_ptr<BoxCollider> collider_;
	std::unique_ptr<ModelInstance> model_;
	Desc desc_;
};

class DropGimmick {
public:
	struct Desc {
		Switch::Desc switchDesc;
		Dropper::Desc dropperDesc;
	};
	void Initialize(const Desc& desc);
	void Update();
private:
	std::vector<std::unique_ptr<Switch>>switch_;
	std::vector<std::unique_ptr<Dropper>>dropper_;


};