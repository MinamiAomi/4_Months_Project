#include "FireBarEditor.h"

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
#include "Externals/ImGui/imgui.h"

void FireBarEditor::Initialize() {
	fileName_ = "FireBar";

	transform.scale = Vector3::one;
	transform.rotate = Quaternion::MakeFromEulerAngle(Vector3::zero);
	transform.translate = Vector3::zero;

	desc_.desc.transform.translate = transform.translate;
	desc_.desc.transform.scale = transform.scale;
	desc_.barDesc.barInitialAngle = 90.0f * Math::ToRadian;
	desc_.barDesc.length = 1.0f;
	desc_.barDesc.rotateVelocity = 0.01f;

	fireBar_ = std::make_unique<FireBar>();
	fireBar_->SetCamera(camera_);
	fireBar_->SetPlayer(player_);
	fireBar_->Initialize(desc_);
	fireBar_->SetIsActive(false);
}

void FireBarEditor::Update() {
#ifdef _DEBUG
	ImGui::Begin("StageEditor");
	if (ImGui::TreeNode("FireBarEditor")) {
	/*	if (ImGui::TreeNode("CreateFireBar")) {
			fireBar_->SetIsActive(true);
			ImGui::DragFloat3("position", &transform.translate.x, 0.25f);

			if (ImGui::TreeNode("Center")) {
				ImGui::DragFloat3("scale", &transform.scale.x, 0.25f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Bar")) {
				ImGui::DragFloat("length:", &desc_.barDesc.length, 0.1f);
				ImGui::DragFloat("barRotateVelocity:", &desc_.barDesc.rotateVelocity, 0.01f);
				desc_.barDesc.barInitialAngle *= Math::ToDegree;
				ImGui::DragFloat("InitialAngle:", &desc_.barDesc.barInitialAngle, 1.0f);
				desc_.barDesc.barInitialAngle *= Math::ToRadian;
				ImGui::TreePop();
			}
			desc_.desc.transform.translate = transform.translate;
			desc_.desc.transform.scale = transform.scale;
			fireBar_->SetDesc(desc_);
			if (ImGui::Button("Create")) {
				fireBarManager_->Create(desc_);
			}
			ImGui::TreePop();
			isCreate_ = true;
			fireBar_->Update();
			UpdateTransform();
		}
		else {
			fireBar_->SetIsActive(false);
			isCreate_ = false;
		}*/
		for (uint32_t i = 0; auto & fireBar : fireBarManager_->GetFireBars()) {
			if (fireBar.get() == nullptr) {
				continue;
			}
			if (ImGui::TreeNode(("FireBar:" + std::to_string(i)).c_str())) {
				auto& desc = fireBar->GetDesc();
				ImGui::DragFloat3(("pos:" + std::to_string(i)).c_str(), &fireBar->transform.translate.x, 1.0f);
				if (ImGui::TreeNode("Center")) {
					ImGui::DragFloat3(("scale:" + std::to_string(i)).c_str(), &fireBar->transform.scale.x, 0.1f);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Bar")) {
					ImGui::DragFloat(("length:" + std::to_string(i)).c_str(), &desc.barDesc.length, 0.1f);
					ImGui::DragFloat(("barRotateVelocity:" + std::to_string(i)).c_str(), &desc.barDesc.rotateVelocity, 0.01f);
					desc.barDesc.barInitialAngle *= Math::ToDegree;
					ImGui::DragFloat(("InitialAngle:" + std::to_string(i)).c_str(), &desc.barDesc.barInitialAngle, 0.01f);
					desc.barDesc.barInitialAngle *= Math::ToRadian;
					ImGui::TreePop();
				}
				if (ImGui::Button("Delete")) {
					fireBarManager_->DeleteFireBar(fireBar.get());
					ImGui::TreePop();
					break;
				}
				fireBar->SetDesc(desc);
				ImGui::TreePop();
			}
			i++;
		}
		ImGui::TreePop();
	}
	else {
		fireBar_->SetIsActive(false);
		isCreate_ = false;
	}
	ImGui::End();

#endif // _DEBUG
}

void FireBarEditor::SaveFile(uint32_t stageName) {
	stageName;
	//nlohmann::json root;

	//root = nlohmann::json::object();

	//root[fileName_] = nlohmann::json::object();

	//for (size_t i = 0; auto & fireBar : fireBarManager_->GetFireBars()) {
	//	root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["position"] = nlohmann::json::array({ fireBar->transform.translate.x, fireBar->transform.translate.y, fireBar->transform.translate.z });
	//	root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["scale"] = nlohmann::json::array({ fireBar->transform.scale.x, fireBar->transform.scale.y, fireBar->transform.scale.z });
	//	root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["length"] = fireBar->GetDesc().barDesc.length;
	//	root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["rotateVelocity"] = fireBar->GetDesc().barDesc.rotateVelocity;
	//	root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["barRotate"] = nlohmann::json::array({ fireBar->GetDesc().barDesc.barInitialAngle.x, fireBar->GetDesc().barDesc.barInitialAngle.y, fireBar->GetDesc().barDesc.barInitialAngle.z });
	//	i++;
	//}

	//const std::filesystem::path kDirectoryPath = "Resources/Data/" + fileName_ + "/" + std::to_string(stageName);

	//if (!std::filesystem::exists(kDirectoryPath)) {
	//	std::filesystem::create_directories(kDirectoryPath);
	//}
	//std::string filePath = kDirectoryPath.string() + std::string(".json");

	//std::ofstream file(filePath);

	//if (file.fail()) {
	//	assert(!"fileSaveFailed");
	//	return;
	//}

	//file << std::setw(4) << root << std::endl;

	//file.close();
}

void FireBarEditor::LoadFile(uint32_t stageName) {
	stageName;
	fireBarManager_->LoadJson(stageName);
}

void FireBarEditor::Clear() {
	fireBarManager_->Clear();
}

void FireBarEditor::UpdateTransform() {
	transform.UpdateMatrix();
}

void FireBarEditor::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}
