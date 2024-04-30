#include "Culling.h"

#include "Engine/Math/Transform.h"
#include "Engine/Math/MathUtils.h"
#include "Graphics/Model.h"
#include "Math/Geometry.h"
#include "Math/Camera.h"

bool Culling::Culling(const Transform& transform, const ModelInstance& model, const Camera& camera) {
	Vector3 modelSize = (model.GetModel()->GetMeshes().at(0).maxVertex - model.GetModel()->GetMeshes().at(0).minVertex);
	Math::Sphere modelSphere{}, cameraSphere{};
	modelSphere.center = (transform.worldMatrix.GetTranslate());
	modelSphere.radius = ((std::max)(modelSize.z * transform.scale.z, std::max(modelSize.x * transform.scale.x, modelSize.y * transform.scale.y)));
	cameraSphere.center = (camera.GetPosition());
	cameraSphere.radius = (camera.GetFarClip());
	if (Math::IsCollision(modelSphere, cameraSphere)) {
		return true;
	}
	return false;
}
