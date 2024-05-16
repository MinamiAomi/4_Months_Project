#include "StageGimmick.h"

#include "File/JsonConverter.h"

StageGimmick::Desc StageGimmick::GetDesc(const nlohmann::json& obj) {
	StageGimmick::Desc desc{};
	desc.name = obj["file_name"];

	const auto& transform = obj["transform"];
	desc.transform.scale = transform["scale"].get<Vector3>();
	desc.transform.rotate = transform["rotate"].get<Quaternion>();
	desc.transform.translate = transform["translate"].get<Vector3>();

	if (obj.contains("collider")) {
		desc.collider = Collider{};
		const auto& collider = obj["collider"];
		desc.collider->center = { collider["center"][0], collider["center"][1], collider["center"][2] };
		desc.collider->rotate = Quaternion{ collider["rotate"][0], collider["rotate"][1], collider["rotate"][2] ,collider["rotate"][3] };
		desc.collider->size = { collider["size"][0], collider["size"][1], collider["size"][2] };
	}
	return desc;
}

void StageGimmick::SetDesc(Transform& transform, std::optional<Collider>& collider, const Desc& desc) {
	transform.scale = desc.transform.scale;
	transform.rotate = desc.transform.rotate;
	transform.translate = desc.transform.translate;

	collider = desc.collider;
}
