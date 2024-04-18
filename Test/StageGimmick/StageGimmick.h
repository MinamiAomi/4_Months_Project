#pragma once

#include <string>
#include <optional>

#include "Engine/Math/MathUtils.h"

#include "Externals/nlohmann/json.hpp"

namespace StageGimmick {
	struct Collider {
		Vector3 center;
		Vector3 size;
		Vector3 rotate;
	};
	struct Transform {
		Vector3 translate;
		Vector3 rotate;
		Vector3 scale;
	};

	struct Desc {
		std::optional<Collider> collider;
		Transform transform;
		std::string name;
	};

	static const std::string stageScenePath_ = "Resources/Data/StageScene/stageScene.json";

	Desc GetDesc(const nlohmann::json& json);
}