#include "PendulumEditor.h"

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
#include "Externals/ImGui/imgui.h"

const std::string PendulumEditor::kModelName = "block";
void PendulumEditor::Initialize() {
	fileName_ = "Pendulum";

	pendulum_ = std::make_unique<Pendulum>();
	Pendulum::Desc desc{};
	desc.length = 15.0f;
	desc.desc.transform.translate = { 0.0f,desc.length ,0.0f };
	desc.desc.transform.scale = { 3.0f,3.0f,3.0f };
	desc.gravity = { 0.002f };
	desc.angle = 0.0f * Math::ToRadian;
	desc.initializeAngle = desc.angle;
	desc.ballScale = 1.0f;
	desc.stickScale = 1.0f;
	pendulum_->SetCamera(camera_);
	pendulum_->SetPlayer(player_);
	pendulum_->Initialize(desc);
#
	// 一回だけ実行
	pendulum_->Update();
	pendulum_->SetIsActive(false);
}

void PendulumEditor::Update() {
#ifdef _DEBUG


	static bool isPlay = false;
	ImGui::Begin("StageEditor");
	if (ImGui::TreeNode("PendulumEditor")) {
		/*if (ImGui::TreeNode("CreatePendulum")) {
			pendulum_->SetIsActive(true);
			auto desc = pendulum_->GetDesc();
			ImGui::DragFloat3("position", &desc.desc.transform.translate.x, 0.1f);
			ImGui::DragFloat("length", &desc.length, 0.1f);
			ImGui::DragFloat("gravity", &desc.gravity, 0.001f);
			desc.angle *= Math::ToDegree;
			ImGui::DragFloat("angle:", &desc.angle, 0.1f);
			desc.angle *= Math::ToRadian;
			desc.initializeAngle *= Math::ToDegree;
			ImGui::DragFloat("initializeAngle:", &desc.initializeAngle, 0.1f);
			desc.initializeAngle *= Math::ToRadian;
			if (ImGui::TreeNode("Stick")) {
				ImGui::DragFloat("scale", &desc.stickScale, 0.1f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Ball")) {
				ImGui::DragFloat("scale", &desc.ballScale, 0.1f);
				ImGui::TreePop();
			}
			if (!isPlay) {
				pendulum_->SetDesc(desc);
			}
			ImGui::Checkbox("Play", &isPlay);
			if (ImGui::Button("Create")) {
				pendulumManager_->Create(pendulum_->GetDesc());
			}
			ImGui::TreePop();
			isCreate_ = true;
		}
		else {
			pendulum_->SetIsActive(false);
			isCreate_ = false;
		}*/
		for (uint32_t i = 0; auto & pendulum : pendulumManager_->GetPendulums()) {
			if (pendulum.get() == nullptr) {
				continue;
			}
			if (ImGui::TreeNode(("Pendulum:" + std::to_string(i)).c_str())) {
				auto desc = pendulum->GetDesc();
				ImGui::DragFloat3(("pos:" + std::to_string(i)).c_str(), &desc.desc.transform.translate.x, 1.0f);
				ImGui::DragFloat(("length:" + std::to_string(i)).c_str(), &desc.length, 0.01f);
				ImGui::DragFloat(("gravity:" + std::to_string(i)).c_str(), &desc.gravity, 0.001f);
				desc.angle *= Math::ToDegree;
				ImGui::DragFloat(("angle:" + std::to_string(i)).c_str(), &desc.angle, 0.1f);
				desc.angle *= Math::ToRadian;
				desc.initializeAngle *= Math::ToDegree;
				ImGui::DragFloat("initializeAngle:", &desc.initializeAngle, 0.1f);
				desc.initializeAngle *= Math::ToRadian;
				if (ImGui::TreeNode("Stick")) {
					ImGui::DragFloat(("scale:" + std::to_string(i)).c_str(), &desc.stickScale, 0.1f);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Ball")) {
					ImGui::DragFloat(("scale:" + std::to_string(i)).c_str(), &desc.ballScale, 0.1f);
					ImGui::TreePop();
				}
				pendulum->SetDesc(desc);
				if (ImGui::Button("Delete")) {
					pendulumManager_->DeletePendulum(pendulum.get());
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
		pendulum_->SetIsActive(false);
	}
	ImGui::End();
	if (isPlay) {
		pendulum_->Update();
	}
#endif // _DEBUG
}

void PendulumEditor::SaveFile(uint32_t stageName) {
	stageName;
	/*nlohmann::json root;

	root = nlohmann::json::object();

	root[fileName_] = nlohmann::json::object();

	for (size_t i = 0; auto & pendulum : pendulumManager_->GetPendulums()) {
		auto desc = pendulum->GetDesc();
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["position"] = nlohmann::json::array({ desc.pos.x, desc.pos.y, desc.pos.z });
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["scale"] = nlohmann::json::array({ desc.scale.x, desc.scale.y, desc.scale.z });
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["ballScale"] = desc.ballScale;
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["stickScale"] = desc.stickScale;
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["length"] = desc.length;
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["gravity"] = desc.gravity;
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["angle"] = desc.angle;
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["initializeAngle"] = desc.initializeAngle;
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

void PendulumEditor::LoadFile(uint32_t stageName) {
	pendulumManager_->LoadJson(stageName);
}

void PendulumEditor::Clear() {
	pendulumManager_->Clear();
}

void PendulumEditor::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

