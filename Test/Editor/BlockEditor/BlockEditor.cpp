#include "BlockEditor.h"

#include <Windows.h>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <list>
#include <vector>

#include "CollisionAttribute.h"
#include "Externals/nlohmann/json.hpp"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

const std::string BlockEditor::kModelName = "block";

void BlockEditor::Initialize() {
	fileName_ = "Block";

	model_ = std::make_unique<ModelInstance>();

	rotate_ = Vector3::zero;

	transform.scale = Vector3::one;
	transform.rotate = Quaternion::MakeFromEulerAngle(Vector3::zero);
	transform.translate = Vector3::zero;

	model_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));

#ifdef _DEBUG
	model_->SetIsActive(true);
#else
	model_->SetIsActive(false);
#endif // _DEBUG


#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Block");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Block);
	collider_->SetCollisionMask(~CollisionAttribute::Block);
#ifdef _DEBUG
	collider_->SetIsActive(true);
#else
	collider_->SetIsActive(false);
#endif // _DEBUG
#pragma endregion
}

void BlockEditor::Update() {
#ifdef _DEBUG


	ImGui::Begin("StageEditor");
	if (ImGui::TreeNode("BlockEditor")) {
	/*	static bool isCollision = true;
		if (ImGui::TreeNode("CreateBlock")) {
			model_->SetIsActive(true);
			collider_->SetIsActive(true);
			ImGui::DragFloat3("scale", &transform.scale.x, 0.25f);
			ImGui::DragFloat3("rotate", &rotate_.x, 0.01f);
			transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
			ImGui::DragFloat3("position", &transform.translate.x, 0.25f);
			if (ImGui::Button("Create")) {
				blockManager_->Create(transform.scale, rotate_, transform.translate);
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
		Vector3 blockScale{}, blockRotate{}, blockPos{};
		for (uint32_t i = 0; auto & block : blockManager_->GetBlocks()) {
			if (block.get() == nullptr) {
				continue;
			}
			if (ImGui::TreeNode(("Block:" + std::to_string(i)).c_str())) {
				ImGui::DragFloat3(("scale:" + std::to_string(i)).c_str(), &block->transform.scale.x, 0.1f);
				//ImGui::DragFloat3(("rotate:" + std::to_string(i)).c_str(), &block->transform.rotate.x, 0.01f);
				ImGui::DragFloat3(("position:" + std::to_string(i)).c_str(), &block->transform.translate.x, 1.0f);

				if (ImGui::Button("Delete")) {
					blockManager_->DeleteBlock(block.get());
					ImGui::TreePop();
					break;
				}

				ImGui::TreePop();
			}
			i++;
		}

		ImGui::TreePop();
	}
	else {
		model_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
	ImGui::End();
#endif // DEBUG
	UpdateTransform();
}

void BlockEditor::SaveFile(uint32_t stageName) {
	stageName;
	/*nlohmann::json root;

	root = nlohmann::json::object();

	root[fileName_] = nlohmann::json::object();

	for (size_t i = 0; auto & blockData : blockManager_->GetBlocks()) {
		root[fileName_]["objectData"][("Block:" + std::to_string(i)).c_str()]["scale"] = nlohmann::json::array({ blockData->GetScale().x, blockData->GetScale().y, blockData->GetScale().z });
		root[fileName_]["objectData"][("Block:" + std::to_string(i)).c_str()]["rotate"] = nlohmann::json::array({ blockData->GetRotate().x, blockData->GetRotate().y, blockData->GetRotate().z });
		root[fileName_]["objectData"][("Block:" + std::to_string(i)).c_str()]["position"] = nlohmann::json::array({ blockData->GetPosition().x, blockData->GetPosition().y, blockData->GetPosition().z });

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

void BlockEditor::LoadFile(uint32_t stageName) {
	blockManager_->LoadJson(stageName);
}

void BlockEditor::Clear() {
	blockManager_->Clear();
}

void BlockEditor::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	Vector3 modelSize = (model_->GetModel()->GetMeshes().at(0).maxVertex - model_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void BlockEditor::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}
