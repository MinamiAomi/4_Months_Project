#pragma once

#include <string>

#include "Engine/Math/MathUtils.h"

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
		Collider collider;
		Transform transform;
		std::string name;
	};

	static const std::string stageScenePath_ = "Resources/Data/StageScene/stageScene.json";
}