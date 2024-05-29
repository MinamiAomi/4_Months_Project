#include "FloorEditor.h"

#include <Windows.h>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <list>
#include <vector>

#include "CollisionAttribute.h"
#include "Externals/nlohmann/json.hpp"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

const std::string FloorEditor::kModelName = "floor";

void FloorEditor::Initialize() {
	fileName_ = "Floor";

	model_ = std::make_unique<ModelInstance>();

	rotate_ = Vector3::zero;

	transform.scale = Vector3::one;
	transform.rotate = Quaternion::MakeFromEulerAngle(Vector3::zero);
	transform.translate = { 0.0f,-3.0f,0.0f };

	model_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));
#ifdef _DEBUG
	model_->SetIsActive(false);
#else
	model_->SetIsActive(false);
#endif // _DEBUG

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Floor");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	//collider_->SetCollisionAttribute(CollisionAttribute::Floor);
	//collider_->SetCollisionMask(~CollisionAttribute::Floor);

#ifdef _DEBUG
	collider_->SetIsActive(false);
#else
	collider_->SetIsActive(false);
#endif // _DEBUG

#pragma endregion

}

void FloorEditor::Update() {
#ifdef _DEBUG

	ImGui::Begin("StageEditor");
	if (ImGui::TreeNode("FloorEditor")) {
		/*	static bool isCollision = false;
			if (ImGui::TreeNode("CreateFloor")) {
				model_->SetIsActive(true);
				collider_->SetIsActive(true);
				ImGui::DragFloat3("scale", &transform.scale.x, 0.25f);
				ImGui::DragFloat3("position", &transform.translate.x, 0.25f);
				if (ImGui::Button("Create")) {
					StageGimmick::Desc desc{};
					desc.transform.translate = transform.translate;
					desc.transform.rotate = rotate_;
					desc.transform.scale = transform.scale;
					desc.name = "floor";
					floorManager_->Create(desc);
				}
				ImGui::TreePop();
				isCreate_ = true;
			}
			else {
				model_->SetIsActive(false);
				collider_->SetIsActive(false);
				isCreate_ = false;
			}
			if (ImGui::Checkbox("isCollision", &isCollision)) {
				collider_->SetIsActive(isCollision);
			}*/
		for (uint32_t i = 0; auto & floor : floorManager_->GetFloors()) {
			if (floor.get() == nullptr) {
				continue;
			}
			if (ImGui::TreeNode(("Floor:" + std::to_string(i)).c_str())) {

				ImGui::DragFloat3(("scale:" + std::to_string(i)).c_str(), &floor->transform.scale.x, 0.1f);
				ImGui::DragFloat3(("position:" + std::to_string(i)).c_str(), &floor->transform.translate.x, 1.0f);

				if (ImGui::Button("Delete")) {
					floorManager_->DeleteFloor(floor.get());
					ImGui::TreePop();
					break;
				}

				ImGui::TreePop();
			}
			i++;
		}
		ImGui::TreePop();
	}
	ImGui::End();
#endif // _DEBUG
	UpdateTransform();
}

void FloorEditor::SaveFile(uint32_t stageName) {
	stageName;
	/*nlohmann::json root;

	root = nlohmann::json::object();

	root[fileName_] = nlohmann::json::object();

	for (size_t i = 0; auto & floorData : floorManager_->GetFloors()) {
		root[fileName_]["objectData"][("Floor:" + std::to_string(i)).c_str()]["scale"] = nlohmann::json::array({ floorData->GetScale().x, floorData->GetScale().y, floorData->GetScale().z });
		root[fileName_]["objectData"][("Floor:" + std::to_string(i)).c_str()]["rotate"] = nlohmann::json::array({ floorData->GetRotate().x, floorData->GetRotate().y, floorData->GetRotate().z });
		root[fileName_]["objectData"][("Floor:" + std::to_string(i)).c_str()]["position"] = nlohmann::json::array({ floorData->GetPosition().x, floorData->GetPosition().y, floorData->GetPosition().z });

		i++;
	}

	const std::filesystem::path kDirectoryPath = "Resources/Data/" + fileName_ + "/" + std::to_string(stageName);

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}
	std::string filePath = kDirectoryPath.string() + std::string(".json");

	std::ofstream file(filePath);

	if (file.fail()) {
		assert(!"fileSaveFailed");
		return;
	}

	file << std::setw(4) << root << std::endl;

	file.close();*/
}

void FloorEditor::Clear() {
	floorManager_->Clear();
}

void FloorEditor::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * scale.x,modelSize.y * scale.y ,modelSize.z * scale.z });
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void FloorEditor::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}
