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

	stick_ = std::make_unique<ModelInstance>();
	ball_ = std::make_unique<ModelInstance>();

	pendulum_.anchor = { 0.0f,1.0f,0.0f };
	pendulum_.length = 0.8f;
	pendulum_.angle = 0.7f;
	pendulum_.gravity = 0.002f;
	pendulum_.angularVelocity = 0.0f;
	pendulum_.angularAcceleration = 0.0f;

	speed_ = 0.01f;
	angle_ = 15.0f;
	length_ = 5.0f;
	pos_ = { 0.0f,15.0f,0.0f };
	rotate_ = Vector3::zero;
	ballRotate_ = Vector3::zero;

	transform.scale = { 5.0f,1.0f,1.0f };
	transform.rotate = Quaternion::MakeFromEulerAngle(Vector3{ 0.0f, 270.0f * Math::ToRadian, 0.0f });
	transform.translate = transform.rotate.GetForward() * transform.scale.z * 0.5f;
	ballTransform_.scale = { 3.0f,3.0f,3.0f };
	ballTransform_.rotate = Quaternion::MakeFromEulerAngle(Vector3::zero);
	ballTransform_.translate = transform.rotate.GetForward() * (transform.scale.z + ballTransform_.scale.z);

	stick_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));
	ball_->SetModel(ResourceManager::GetInstance()->FindModel(kModelName));
	stick_->SetIsActive(true);
	ball_->SetIsActive(true);

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("PendulumBall");
	collider_->SetCenter(ballTransform_.translate);
	collider_->SetOrientation(ballTransform_.rotate);
	collider_->SetSize(ballTransform_.scale);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::PendulumBall);
	collider_->SetCollisionMask(~CollisionAttribute::PendulumBall);
	collider_->SetIsActive(true);
#pragma endregion
}

void PendulumEditor::Update() {
	ImGui::Begin("StageEditor");
	if (ImGui::TreeNode("PendulumEditor")) {
		stick_->SetIsActive(true);
		ball_->SetIsActive(true);
		static bool isCollision = true;
		if (ImGui::Checkbox("isCollision", &isCollision)) {
			collider_->SetIsActive(isCollision);
		}
		Vector3 pos{}, stickRotate{}, stickScale{}, ballRotate{}, ballScale{};
		float speed, angle;
		for (uint32_t i = 0; auto & pendulum : pendulumManager_->GetPendulums()) {
			if (pendulum.get() == nullptr) {
				continue;
			}
			if (ImGui::TreeNode(("Pendulum:" + std::to_string(i)).c_str())) {
				pos = pendulum->GetPosition();
				stickRotate = pendulum->GetStickRotate();
				stickScale = pendulum->GetStickScale();
				ballRotate = pendulum->GetBallRotate();
				ballScale = pendulum->GetBallScale();
				angle = pendulum->GetAngle();
				speed = pendulum->GetSpeed();
				ImGui::DragFloat3(("pos:" + std::to_string(i)).c_str(), &pos.x, 1.0f);
				ImGui::DragFloat(("speed:" + std::to_string(i)).c_str(), &speed, 0.1f);
				ImGui::DragFloat(("angle:" + std::to_string(i)).c_str(), &angle, 1.0f);
				if (ImGui::TreeNode("Stick")) {
					ImGui::DragFloat3(("scale:" + std::to_string(i)).c_str(), &stickScale.x, 0.1f);
					ImGui::DragFloat3(("rotate:" + std::to_string(i)).c_str(), &stickRotate.x, 0.01f);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Ball")) {
					ImGui::DragFloat3(("scale:" + std::to_string(i)).c_str(), &ballScale.x, 0.1f);
					ImGui::DragFloat3(("rotate:" + std::to_string(i)).c_str(), &ballRotate.x, 0.01f);
					ImGui::TreePop();
				}
				pendulum->SetStickScale(stickScale);
				pendulum->SetStickRotate(stickRotate);
				pendulum->SetBallScale(ballScale);
				pendulum->SetBallRotate(ballRotate);
				pendulum->SetSpeed(speed);
				pendulum->SetAngle(angle);
				pendulum->SetPosition(pos);
				if (ImGui::Button("Delete")) {
					pendulumManager_->DeletePendulum(pendulum.get());
					ImGui::TreePop();
					break;
				}

				ImGui::TreePop();
			}
			i++;
		}
		if (ImGui::TreeNode("CreateBlock")) {
			ImGui::DragFloat3("position", &pos_.x, 0.25f);
			ImGui::DragFloat("speed", &speed_, 0.01f);
			ImGui::DragFloat("angle", &angle_, 1.0f);
			if (ImGui::TreeNode("Stick")) {
				ImGui::DragFloat3("scale", &transform.scale.x, 0.25f);
				ImGui::DragFloat3("rotate", &rotate_.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Ball")) {
				ImGui::DragFloat3("scale", &ballTransform_.scale.x, 0.1f);
				ImGui::DragFloat3("rotate", &ballRotate_.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::Button("Create")) {
				pendulumManager_->Create(transform.scale, rotate_, ballTransform_.scale, ballRotate_, pos_, length_, speed_, angle_);
			}
			ImGui::TreePop();
			isCreate_ = true;
		}
		else {
			isCreate_ = false;
		}
		ImGui::TreePop();
	}
	else {
		stick_->SetIsActive(false);
		ball_->SetIsActive(false);
		collider_->SetIsActive(false);
	}
	ImGui::End();
	static bool  clockwise = true;
	if (clockwise) {
		rotate_.z += speed_;
	}
	else {
		rotate_.z -= speed_;
	}
	if (std::fabs(rotate_.z) >= angle_ * Math::ToRadian) {
		clockwise ^= true;
	}
	ImGui::Begin("pendulum");
	ImGui::DragFloat3("anchor",&pendulum_.anchor.x,0.1f);
	ImGui::DragFloat("length",&pendulum_.length,0.1f);
	ImGui::DragFloat("angle",&pendulum_.angle,0.1f);
	ImGui::DragFloat("gravity",&pendulum_.gravity,0.001f);
	ImGui::DragFloat("angularVelocity",&pendulum_.angularVelocity,0.1f);
	ImGui::DragFloat("angularAcceleration",&pendulum_.angularAcceleration,0.1f);
	ImGui::End();
	pendulum_.Update();
	UpdateTransform();
}

void PendulumEditor::SaveFile(uint32_t stageName) {
	nlohmann::json root;

	root = nlohmann::json::object();

	root[fileName_] = nlohmann::json::object();

	for (size_t i = 0; auto & pendulum : pendulumManager_->GetPendulums()) {
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["position"] = nlohmann::json::array({ pendulum->GetPosition().x, pendulum->GetPosition().y, pendulum->GetPosition().z });
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["stickScale"] = nlohmann::json::array({ pendulum->GetStickScale().x, pendulum->GetStickScale().y, pendulum->GetStickScale().z });
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["stickRotate"] = nlohmann::json::array({ pendulum->GetStickRotate().x, pendulum->GetStickRotate().y, pendulum->GetStickRotate().z });
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["ballScale"] = nlohmann::json::array({ pendulum->GetBallScale().x, pendulum->GetBallScale().y, pendulum->GetBallScale().z });
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["ballRotate"] = nlohmann::json::array({ pendulum->GetBallRotate().x, pendulum->GetBallRotate().y, pendulum->GetBallRotate().z });
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["speed"] = pendulum->GetSpeed();
		root[fileName_]["objectData"][("Pendulum:" + std::to_string(i)).c_str()]["angle"] = pendulum->GetAngle();
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

				std::vector<Vector3> pos{}, stickRotate{}, stickScale{}, ballRotate{}, ballScale{};
				std::vector<float> speed, angle;
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
						else if (itemNameObject == "stickRotate") {
							//float型のjson配列登録
							stickRotate.emplace_back(Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
						//名前がscaleだった場合、scaleを登録
						else if (itemNameObject == "stickScale") {
							//float型のjson配列登録
							stickScale.emplace_back(Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
						else if (itemNameObject == "ballRotate") {
							//float型のjson配列登録
							ballRotate.emplace_back(Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
						//名前がscaleだった場合、scaleを登録
						else if (itemNameObject == "ballScale") {
							//float型のjson配列登録
							ballScale.emplace_back(Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
					}
					else {
						if (itemNameObject == "speed") {
							float sp = itItemObject->get<float>();
							speed.emplace_back(sp);
						}
						else if (itemNameObject == "angle") {
							float an = itItemObject->get<float>();
							angle.emplace_back(an);
						}
					}
				}

				// 生成
				for (size_t i = 0; i < pos.size(); i++) {
					pendulumManager_->Create(stickScale.at(i), stickRotate.at(i), ballScale.at(i), ballRotate.at(i), pos.at(i), 0.0f, speed.at(i), angle.at(i));
				}
			}
		}
	}
}

void PendulumEditor::Clear() {
	pendulumManager_->Clear();
}

void PendulumEditor::UpdateTransform() {
	transform.translate = pendulum_.GetPosition();
	transform.UpdateMatrix();
	stick_->SetWorldMatrix(transform.worldMatrix);

	ballTransform_.rotate = Quaternion::MakeFromEulerAngle(ballRotate_);
	ballTransform_.translate = transform.rotate.GetForward() * (transform.scale.x + ballTransform_.scale.x);

	ballTransform_.UpdateMatrix();
	ball_->SetWorldMatrix(ballTransform_.worldMatrix);
	Vector3 scale, translate;
	Quaternion rotate;
	ballTransform_.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	collider_->SetSize(scale);
	collider_->SetOrientation(rotate);
}

void PendulumEditor::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
}

