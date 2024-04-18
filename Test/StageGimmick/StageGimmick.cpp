#include "StageGimmick.h"

StageGimmick::Desc StageGimmick::GetDesc(const nlohmann::json& obj) {
	StageGimmick::Desc desc{};
	desc.name = obj["file_name"];

	const auto& transform = obj["transform"];
	desc.transform.scale = { transform["scale"][0], transform["scale"][1], transform["scale"][2] };
	desc.transform.rotate = { transform["rotate"][0], transform["rotate"][1], transform["rotate"][2] };
	desc.transform.translate = { transform["translate"][0], transform["translate"][1], transform["translate"][2] };

	if (obj.contains("collider")) {
		desc.collider = Collider{};
		const auto& collider = obj["collider"];
		desc.collider->center = { collider["center"][0], collider["center"][1], collider["center"][2] };
		desc.collider->rotate = { collider["rotate"][0], collider["rotate"][1], collider["rotate"][2] };
		desc.collider->size = { collider["size"][0], collider["size"][1], collider["size"][2] };
	}
	return desc;
}
