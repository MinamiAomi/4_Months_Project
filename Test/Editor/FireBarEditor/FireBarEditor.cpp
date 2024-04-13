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

	desc_.transform = transform;
	desc_.barDesc.barInitialAngle.y = 90.0f * Math::ToRadian;
	desc_.barDesc.length = 1.0f;
	desc_.barDesc.rotateVelocity = 0.01f;

	fireBar_ = std::make_unique<FireBar>();
	fireBar_->SetPlayer(player_);
	fireBar_->Initialize(desc_);
	fireBar_->SetIsActive(false);
}

void FireBarEditor::Update() {
#ifdef _DEBUG


	ImGui::Begin("StageEditor");
	if (ImGui::TreeNode("FireBarEditor")) {
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
					ImGui::DragFloat(("InitialAngle:" + std::to_string(i)).c_str(), &desc.barDesc.barInitialAngle.y, 0.01f);
					desc.barDesc.barInitialAngle *= Math::ToRadian;
					ImGui::TreePop();
				}
				if (ImGui::Button("Delete")) {
					fireBarManager_->DeleteFireBar(fireBar.get());
					ImGui::TreePop();
					break;
				}

				ImGui::TreePop();
			}
			i++;
		}
		if (ImGui::TreeNode("CreateFireBar")) {
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
				ImGui::DragFloat("InitialAngle:", &desc_.barDesc.barInitialAngle.y, 0.01f);
				desc_.barDesc.barInitialAngle *= Math::ToRadian;
				ImGui::TreePop();
			}
			desc_.transform = transform;
			fireBar_->SetDesc(desc_);
			if (ImGui::Button("Create")) {
				fireBarManager_->Create(desc_);
			}
			ImGui::TreePop();
			isCreate_ = true;
		}
		else {
			fireBar_->SetIsActive(false);
			isCreate_ = false;
		}
		ImGui::TreePop();
	}
	else {
		fireBar_->SetIsActive(false);
		isCreate_ = false;
	}
	ImGui::End();
#endif // _DEBUG
	fireBar_->Update();
	UpdateTransform();
}

void FireBarEditor::SaveFile(uint32_t stageName) {
	nlohmann::json root;

	root = nlohmann::json::object();

	root[fileName_] = nlohmann::json::object();

	for (size_t i = 0; auto & fireBar : fireBarManager_->GetFireBars()) {
		root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["position"] = nlohmann::json::array({ fireBar->transform.translate.x, fireBar->transform.translate.y, fireBar->transform.translate.z });
		root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["scale"] = nlohmann::json::array({ fireBar->transform.scale.x, fireBar->transform.scale.y, fireBar->transform.scale.z });
		root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["length"] = fireBar->GetDesc().barDesc.length;
		root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["rotateVelocity"] = fireBar->GetDesc().barDesc.rotateVelocity;
		root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["barRotate"] = nlohmann::json::array({ fireBar->GetDesc().barDesc.barInitialAngle.x, fireBar->GetDesc().barDesc.barInitialAngle.y, fireBar->GetDesc().barDesc.barInitialAngle.z });
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

void FireBarEditor::LoadFile(uint32_t stageName) {
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

				FireBar::Desc desc{};
				for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {

					//アイテム名を取得
					const std::string& itemNameObject = itItemObject.key();

					//要素数3の配列であれば
					if (itItemObject->is_array() && itItemObject->size() == 3) {

						//名前がpositionだった場合、positionを登録
						if (itemNameObject == "position") {
							//float型のjson配列登録
							desc.transform.translate = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
						//名前がscaleだった場合、scaleを登録
						else if (itemNameObject == "scale") {
							//float型のjson配列登録
							desc.transform.scale = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
						else if (itemNameObject == "barRotate") {
							//float型のjson配列登録
							desc.barDesc.barInitialAngle = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
					}
					else {
						if (itemNameObject == "rotateVelocity") {
							desc.barDesc.rotateVelocity= itItemObject->get<float>();
						}
						else if (itemNameObject == "length") {
							desc.barDesc.length= itItemObject->get<float>();
						}
					}
				}
				fireBarManager_->Create(desc);
			}
		}
	}
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
