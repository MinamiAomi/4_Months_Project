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
#include "Graphics/ResourceManager.h"
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
	collider_->SetCollisionAttribute(CollisionAttribute::Floor);
	collider_->SetCollisionMask(~CollisionAttribute::Floor);
	
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
		static bool isCollision = false;
		if (ImGui::TreeNode("CreateFloor")) {
			model_->SetIsActive(true);
			collider_->SetIsActive(true);
			ImGui::DragFloat3("scale", &transform.scale.x, 0.25f);
			ImGui::DragFloat3("rotate", &rotate_.x, 0.01f);
			transform.rotate = Quaternion::MakeFromEulerAngle(rotate_);
			ImGui::DragFloat3("position", &transform.translate.x, 0.25f);
			if (ImGui::Button("Create")) {
				//floorManager_->Create(transform.scale, rotate_, transform.translate);
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
		}
		Vector3 floorScale{}, floorRotate{}, floorPos{};
		for (uint32_t i = 0; auto & floor : floorManager_->GetFloors()) {
			if (floor.get() == nullptr) {
				continue;
			}
			if (ImGui::TreeNode(("Floor:" + std::to_string(i)).c_str())) {
				floorScale = floor->transform.scale;
				floorRotate = floor->GetRotate();
				floorPos = floor->GetPosition();

				ImGui::DragFloat3(("scale:" + std::to_string(i)).c_str(), &floorScale.x, 0.1f);
				ImGui::DragFloat3(("rotate:" + std::to_string(i)).c_str(), &floorRotate.x, 0.01f);
				ImGui::DragFloat3(("position:" + std::to_string(i)).c_str(), &floorPos.x, 1.0f);

				floor->SetScale(floorScale);
				floor->SetRotate(floorRotate);
				floor->SetPosition(floorPos);
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
	nlohmann::json root;

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

	file.close();
}

void FloorEditor::LoadFile(uint32_t stageName) {
	const std::filesystem::path kDirectoryPath = "Resources/Data/" + fileName_ + "/" + std::to_string(stageName);
	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath.string() + ".json";
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//ファイルオープン失敗したら表示
	if (ifs.fail()) {
		MessageBox(nullptr, L"指定したファイルは存在しません。", L"Map Editor - Load", 0);
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

				std::vector<Vector3>scale{}, rotate{}, pos{};
				for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {

					//アイテム名を取得
					const std::string& itemNameObject = itItemObject.key();

					//要素数3の配列であれば
					if (itItemObject->is_array() && itItemObject->size() == 3) {

						//名前がpositionだった場合、positionを登録
						if (itemNameObject == "position") {
							//float型のjson配列登録
							pos.emplace_back(Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
						//名前がrotationだった場合、rotationを登録
						else if (itemNameObject == "rotate") {
							//float型のjson配列登録
							rotate.emplace_back(Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
						//名前がscaleだった場合、scaleを登録
						else if (itemNameObject == "scale") {
							//float型のjson配列登録
							scale.emplace_back(Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}

					}
				}

				// 生成
				for (size_t i = 0; i < pos.size(); i++) {
					//floorManager_->Create(scale.at(i), rotate.at(i), pos.at(i));
				}
			}
		}
	}
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
