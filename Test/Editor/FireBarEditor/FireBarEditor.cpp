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

const std::string FireBarEditor::kModelName = "block";

void FireBarEditor::Initialize() {
	fileName_ = "FireBar";

	center_ = std::make_unique<ModelInstance>();
	bar_ = std::make_unique<ModelInstance>();

	centerRotate_ = Vector3::zero;
	barRotate_ = Vector3::zero;

	barRotateVelocity_ = 0.01f;

	transform.scale = Vector3::one;
	transform.rotate = Quaternion::MakeFromEulerAngle(Vector3::zero);
	transform.translate = Vector3::zero;
	barTransform_.scale = transform.scale;
	barTransform_.scale.z *= 2.0f;
	barTransform_.rotate = Quaternion::MakeFromEulerAngle(Vector3::zero);
	barTransform_.translate = transform.translate;

	center_->SetModel(ResourceManager::GetInstance()->FindModel("fireBarCenter"));
	bar_->SetModel(ResourceManager::GetInstance()->FindModel("fireBarBar"));

#ifdef _DEBUG
	center_->SetIsActive(true);
	bar_->SetIsActive(true);
#else
	center_->SetIsActive(false);
	bar_->SetIsActive(false);

#endif // _DEBUG



#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("FireBarCenter");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	Vector3 modelSize = (center_->GetModel()->GetMeshes().at(0).maxVertex - center_->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * transform.scale.x,modelSize.y * transform.scale.y ,modelSize.z * transform.scale.z });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::FireBarCenter);
	collider_->SetCollisionMask(~CollisionAttribute::FireBarCenter);
#ifdef _DEBUG
	collider_->SetIsActive(true);
#else
	collider_->SetIsActive(false);

#endif // _DEBUG
#pragma endregion
}

void FireBarEditor::Update() {
#ifdef _DEBUG


	ImGui::Begin("StageEditor");
	if (ImGui::TreeNode("FireBarEditor")) {
		Vector3 pos{}, centerRotate{}, centerScale{}, barRotate{}, barScale{};
		float barRotateVelocity;
		for (uint32_t i = 0; auto & fireBar : fireBarManager_->GetFireBars()) {
			if (fireBar.get() == nullptr) {
				continue;
			}
			if (ImGui::TreeNode(("FireBar:" + std::to_string(i)).c_str())) {
				pos = fireBar->GetPosition();
				centerRotate = fireBar->GetCenter()->GetRotate();
				centerScale = fireBar->GetCenter()->GetScale();
				barRotate = fireBar->GetBar()->GetRotate();
				barScale = fireBar->GetBar()->GetScale();
				barRotateVelocity = fireBar->GetBar()->GetRotateVelocity();
				ImGui::DragFloat3(("pos:" + std::to_string(i)).c_str(), &pos.x, 1.0f);
				if (ImGui::TreeNode("Center")) {
					ImGui::DragFloat3(("scale:" + std::to_string(i)).c_str(), &centerScale.x, 0.1f);
					ImGui::DragFloat3(("rotate:" + std::to_string(i)).c_str(), &centerRotate.x, 0.01f);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Bar")) {
					ImGui::DragFloat3(("scale:" + std::to_string(i)).c_str(), &barScale.x, 0.1f);
					ImGui::DragFloat3(("rotate:" + std::to_string(i)).c_str(), &barRotate.x, 0.01f);
					ImGui::DragFloat(("barRotateVelocity:" + std::to_string(i)).c_str(), &barRotateVelocity, 0.01f);
					ImGui::TreePop();
				}
				fireBar->GetCenter()->SetScale(centerScale);
				fireBar->GetCenter()->SetRotate(centerRotate);
				fireBar->GetBar()->SetScale(barScale);
				fireBar->GetBar()->SetRotate(barRotate);
				fireBar->GetBar()->SetRotateVelocity(barRotateVelocity);
				fireBar->SetPosition(pos);
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
			center_->SetIsActive(true);
			bar_->SetIsActive(true);
			collider_->SetIsActive(true);
			ImGui::DragFloat3("position", &transform.translate.x, 0.25f);

			if (ImGui::TreeNode("Center")) {
				ImGui::DragFloat2("scale", &transform.scale.x, 0.25f);
				ImGui::DragFloat3("rotate", &centerRotate_.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Bar")) {
				ImGui::DragFloat("length", &barTransform_.scale.z, 0.1f);
				ImGui::DragFloat3("rotate", &barRotate_.x, 0.01f);
				ImGui::DragFloat("barRotateVelocity", &barRotateVelocity_, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::Button("Create")) {
				fireBarManager_->Create(transform.translate, transform.scale, centerRotate_, barTransform_.scale, barRotate_, barRotateVelocity_);
			}
			ImGui::TreePop();
			isCreate_ = true;
		}
		else {
			center_->SetIsActive(false);
			bar_->SetIsActive(false);
			collider_->SetIsActive(false);
			isCreate_ = false;
		}
		ImGui::TreePop();
	}
	else {
		center_->SetIsActive(false);
		bar_->SetIsActive(false);
		collider_->SetIsActive(false);
		isCreate_ = false;
	}
	ImGui::End();
#endif // _DEBUG
	barTransform_.translate = transform.translate;
	transform.scale.z = transform.scale.x;
	barTransform_.scale.x = transform.scale.z;
	barTransform_.scale.y = transform.scale.y * 0.8f;
	UpdateTransform();
}

void FireBarEditor::SaveFile(uint32_t stageName) {
	nlohmann::json root;

	root = nlohmann::json::object();

	root[fileName_] = nlohmann::json::object();

	for (size_t i = 0; auto & fireBar : fireBarManager_->GetFireBars()) {
		root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["position"] = nlohmann::json::array({ fireBar->GetPosition().x, fireBar->GetPosition().y, fireBar->GetPosition().z });
		root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["centerScale"] = nlohmann::json::array({ fireBar->GetCenter()->GetScale().x, fireBar->GetCenter()->GetScale().y, fireBar->GetCenter()->GetScale().z });
		root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["centerRotate"] = nlohmann::json::array({ fireBar->GetCenter()->GetRotate().x, fireBar->GetCenter()->GetRotate().y, fireBar->GetCenter()->GetRotate().z });
		root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["barScale"] = nlohmann::json::array({ fireBar->GetBar()->GetScale().x, fireBar->GetBar()->GetScale().y, fireBar->GetBar()->GetScale().z });
		root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["barRotate"] = nlohmann::json::array({ fireBar->GetBar()->GetRotate().x, fireBar->GetBar()->GetRotate().y, fireBar->GetBar()->GetRotate().z });
		root[fileName_]["objectData"][("FireBar:" + std::to_string(i)).c_str()]["berRotateVelocity"] = fireBar->GetBar()->GetRotateVelocity();
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

				std::vector<Vector3> pos{}, centerRotate{}, centerScale{}, barRotate{}, barScale{};
				std::vector<float> rotateRotateVelocity;
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
						else if (itemNameObject == "centerRotate") {
							//float型のjson配列登録
							centerRotate.emplace_back(Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
						//名前がscaleだった場合、scaleを登録
						else if (itemNameObject == "centerScale") {
							//float型のjson配列登録
							centerScale.emplace_back(Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
						else if (itemNameObject == "barRotate") {
							//float型のjson配列登録
							barRotate.emplace_back(Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
						//名前がscaleだった場合、scaleを登録
						else if (itemNameObject == "barScale") {
							//float型のjson配列登録
							barScale.emplace_back(Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
					}
					else {
						if (itemNameObject == "berRotateVelocity") {
							float speed = itItemObject->get<float>();
							rotateRotateVelocity.emplace_back(speed);
						}
					}
				}

				// 生成
				for (size_t i = 0; i < pos.size(); i++) {
					fireBarManager_->Create(pos.at(i), centerScale.at(i), centerRotate.at(i), barScale.at(i), barRotate.at(i), rotateRotateVelocity.at(i));
				}
			}
		}
	}
}

void FireBarEditor::Clear() {
	fireBarManager_->Clear();
}

void FireBarEditor::UpdateTransform() {
	transform.rotate = Quaternion::MakeFromEulerAngle(centerRotate_);
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	collider_->SetOrientation(rotate);
	center_->SetWorldMatrix(transform.worldMatrix);

	barRotate_.y += barRotateVelocity_;
	barRotate_.y = std::fmod(barRotate_.y, Math::Pi * 2.0f);
	Vector3 forward = barTransform_.rotate.GetForward();
	barTransform_.translate = transform.translate + (forward * barTransform_.scale.z * 0.5f);
	barTransform_.rotate = Quaternion::MakeFromEulerAngle(barRotate_);
	barTransform_.UpdateMatrix();
	bar_->SetWorldMatrix(barTransform_.worldMatrix);
}

void FireBarEditor::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}
