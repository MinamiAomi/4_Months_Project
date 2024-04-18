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
#include "Graphics/ResourceManager.h"
#include "Externals/ImGui/imgui.h"

const std::string PendulumEditor::kModelName = "block";
void PendulumEditor::Initialize() {
	fileName_ = "Pendulum";

	pendulum_ = std::make_unique<Pendulum>();
	Pendulum::Desc desc{};
	desc.length = 15.0f;
	desc.pos = { 0.0f,desc.length ,0.0f };
	desc.scale = { 3.0f,3.0f,3.0f };
	desc.gravity = { 0.002f };
	desc.angle = 0.0f * Math::ToRadian;
	desc.initializeAngle = desc.angle;
	desc.ballScale = 1.0f;
	desc.stickScale = 1.0f;
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
		if (ImGui::TreeNode("CreatePendulum")) {
			pendulum_->SetIsActive(true);
			auto desc = pendulum_->GetDesc();
			ImGui::DragFloat3("position", &desc.pos.x, 0.1f);
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
		}
		for (uint32_t i = 0; auto & pendulum : pendulumManager_->GetPendulums()) {
			if (pendulum.get() == nullptr) {
				continue;
			}
			if (ImGui::TreeNode(("Pendulum:" + std::to_string(i)).c_str())) {
				auto desc = pendulum->GetDesc();
				ImGui::DragFloat3(("pos:" + std::to_string(i)).c_str(), &desc.pos.x, 1.0f);
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
	nlohmann::json root;

	root = nlohmann::json::object();

	root[fileName_] = nlohmann::json::object();

	for (size_t i = 0; auto & pendulum : pendulumManager_->GetPendulums()) {
		auto desc = pendulum->GetDesc();
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["position"] = nlohmann::json::array({ desc.pos.x, desc.pos.y, desc.pos.z });
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["ballScale"] = desc.ballScale;
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["stickScale"] = desc.stickScale;
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["length"] = desc.length;
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["gravity"] = desc.gravity;
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["angle"] = desc.angle;
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

	file.close();
}

void PendulumEditor::LoadFile(uint32_t stageName) {
	const std::filesystem::path kDirectoryPath = "Resources/Data/" + fileName_ + "/" + std::to_string(stageName);
	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath.string() + ".json";
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//ファイルオープン失敗したら表示
	if (ifs.fail()) {
		//MessageBox(nullptr, L"指定したファイルは存在しません。", L"Map Editor - Load", 0);
		return;
	}
	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(fileName_);
	//未登録チェック
	if (itGroup == root.end()) {
		//MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
	}
	// アイテム
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテム名を取得
		const std::string& itemName = itItem.key();

		//グループを検索
		nlohmann::json::iterator itObject = itGroup->find(itemName);

		//未登録チェック
		if (itObject == itGroup->end()) {
			//MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
		}

		//保険
		assert(itObject != itGroup->end());//アイテム名がオブジェクトデータだった場合、登録


		if (itemName == "objectData") {

			//各オブジェクトについて
			for (nlohmann::json::iterator itObjectData = itObject->begin(); itObjectData != itObject->end(); ++itObjectData) {

				//アイテム名を取得
				const std::string& objectName = itObjectData.key();

				//グループを検索
				nlohmann::json::iterator itData = itObject->find(objectName);

				//未登録チェック
				if (itData == itObject->end()) {
					//MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
				}

				//保険
				assert(itData != itObject->end());

				Pendulum::Desc desc{};
				for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {
					//アイテム名を取得
					const std::string& itemNameObject = itItemObject.key();

					//要素数3の配列であれば
					if (itItemObject->is_array() && itItemObject->size() == 3) {

						//名前がpositionだった場合、positionを登録
						if (itemNameObject == "position") {
							//float型のjson配列登録
							desc.pos = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
						//名前がrotationだった場合、rotationを登録
						else if (itemNameObject == "scale") {
							//float型のjson配列登録
							desc.scale = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
					}
					else {
						if (itemNameObject == "length") {
							desc.length = itItemObject->get<float>();
						}
						else if (itemNameObject == "gravity") {
							desc.gravity = itItemObject->get<float>();
						}
						else if (itemNameObject == "ballScale") {
							desc.ballScale = itItemObject->get<float>();
						}
						else if (itemNameObject == "stickScale") {
							desc.stickScale = itItemObject->get<float>();
						}
					}
				}
				pendulumManager_->Create(desc);
			}
		}
	}
}

void PendulumEditor::Clear() {
	pendulumManager_->Clear();
}

void PendulumEditor::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

