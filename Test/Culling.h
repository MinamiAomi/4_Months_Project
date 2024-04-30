#pragma once

class ModelInstance;
class Camera;
class Transform;

namespace Culling {
	bool Culling(const Transform& transform, const ModelInstance& model, const Camera& camera);
}
