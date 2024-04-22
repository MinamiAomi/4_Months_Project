#pragma once

#include <string>
#include <optional>

#include "Engine/Math/MathUtils.h"

#include "Externals/nlohmann/json.hpp"
#include "Math/Transform.h"

namespace StageGimmick {
	struct Collider {
		Vector3 center;
		Vector3 size;
		Quaternion rotate;
	};
	struct TransformDesc {
		Vector3 translate;
		Quaternion rotate;
		Vector3 scale;
	};

	struct Desc {
		std::optional<Collider> collider;
		TransformDesc transform;
		std::string name;
	};

	static const std::string stageScenePath_ = "Resources/Data/StageScene/stageScene.json";

	Desc GetDesc(const nlohmann::json& json);
	void SetDesc(Transform& transform, std::optional<Collider>& collider, const Desc& desc);
}