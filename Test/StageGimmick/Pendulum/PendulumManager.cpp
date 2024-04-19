#include "PendulumManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"

void PendulumManager::Initialize(uint32_t stageIndex) {
	Clear();
	LoadJson(stageIndex);
}

void PendulumManager::Update() {
	for (auto& pendulum : pendulums_) {
		pendulum->Update();
	}
}

void PendulumManager::Reset(uint32_t stageIndex) {
	Clear();
	LoadJson(stageIndex);
}

void PendulumManager::Create(Pendulum::Desc desc) {
	Pendulum* pendulum = new Pendulum();
	pendulum->SetCamera(camera_);
	pendulum->SetPlayer(player_);
	pendulum->Initialize(desc);
	pendulums_.emplace_back(std::move(pendulum));
}

void PendulumManager::DeletePendulum(Pendulum* block) {
	// block がリスト内に存在するかを確認し、存在する場合はそのイテレータを取得する
	auto it = std::find_if(pendulums_.begin(), pendulums_.end(), [block](const auto& ptr) {
		return ptr.get() == block;
		});

	// block が見つかった場合は削除する
	if (it != pendulums_.end()) {
		pendulums_.erase(it);
	}
}

void PendulumManager::LoadJson(uint32_t stageIndex) {
	const std::filesystem::path kDirectoryPath = "Resources/Data/Pendulum/" + std::to_string(stageIndex);
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
	nlohmann::json::iterator itGroup = root.find("Pendulum");
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
						else if (itemNameObject == "initializeAngle") {
							desc.initializeAngle = itItemObject->get<float>();
						}
						else if (itemNameObject == "angle") {
							desc.angle = itItemObject->get<float>();
						}
					}
				}
				Create(desc);
			}
		}
	}
}

void PendulumManager::Clear() {
	pendulums_.clear();
}
