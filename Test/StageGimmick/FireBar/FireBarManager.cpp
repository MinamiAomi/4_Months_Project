#include "FireBarManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>


#include "Externals/nlohmann/json.hpp"

void FireBarManager::Initialize(uint32_t stageIndex) {
	fireBars_.clear();
	LoadJson(stageIndex);
}

void FireBarManager::Update() {
	for (auto& fireBar : fireBars_) {
		fireBar->Update();
	}
}

void FireBarManager::Reset(uint32_t stageIndex) {
	Clear();
	LoadJson(stageIndex);
}

void FireBarManager::Create(const Vector3& pos, const Vector3& centerScale, const Vector3& centerRotate, const Vector3& barScale, const Vector3& barRotate, float barRotateVelocity) {
	FireBar* fireBar = new FireBar();
	fireBar->SetPlayer(player_);
	fireBar->Initialize(pos, centerScale, centerRotate, barScale, barRotate, barRotateVelocity);
	fireBars_.emplace_back(std::move(fireBar));
}

void FireBarManager::DeleteFireBar(FireBar* block) {
	// block がリスト内に存在するかを確認し、存在する場合はそのイテレータを取得する
	auto it = std::find_if(fireBars_.begin(), fireBars_.end(), [block](const auto& ptr) {
		return ptr.get() == block;
		});

	// block が見つかった場合は削除する
	if (it != fireBars_.end()) {
		fireBars_.erase(it);
	}
}

void FireBarManager::LoadJson(uint32_t stageIndex) {
	const std::filesystem::path kDirectoryPath = "Resources/Data/FireBar/" + std::to_string(stageIndex);
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
	nlohmann::json::iterator itGroup = root.find("FireBar");
	//未登録チェック
	if (itGroup == root.end()) {
		MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
	}
	// アイテム
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテム名を取得
		const std::string& itemName = itItem.key();

		//グループを検索
		nlohmann::json::iterator itObject = itGroup->find(itemName);

		//未登録チェック
		if (itObject == itGroup->end()) {
			MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
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
					MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
				}

				//保険
				assert(itData != itObject->end());
				if (objectName.find("FireBar") != std::string::npos) {
					Vector3 pos{}, centerRotate{}, centerScale{}, barRotate{}, barScale{};
					float rotateRotateVelocity = 0.0f;
					for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {
						//アイテム名を取得
						const std::string& itemNameObject = itItemObject.key();
						//要素数3の配列であれば
						if (itItemObject->is_array() && itItemObject->size() == 3) {
							//名前がpositionだった場合、positionを登録
							if (itemNameObject == "position") {
								//float型のjson配列登録
								pos = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
							//名前がrotationだった場合、rotationを登録
							else if (itemNameObject == "centerRotate") {
								//float型のjson配列登録
								centerRotate = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
							//名前がscaleだった場合、scaleを登録
							else if (itemNameObject == "centerScale") {
								//float型のjson配列登録
								centerScale = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
							else if (itemNameObject == "barRotate") {
								//float型のjson配列登録
								barRotate = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
							else if (itemNameObject == "barScale") {
								//float型のjson配列登録
								barScale = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
						}
						else {
							if (itemNameObject == "berRotateVelocity") {
								rotateRotateVelocity = itItemObject->get<float>();
							}
						}
					}
					Create(pos, centerScale, centerRotate, barScale, barRotate, rotateRotateVelocity);
				}
			}
		}
	}
}

void FireBarManager::Clear() {
	fireBars_.clear();
}
