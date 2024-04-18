#include "BossAttackTriggerManager.h"

#include <fstream>

#include "Externals/nlohmann/json.hpp"

void BossAttackTriggerManager::Initialize() {
	bossAttackTriggers_.clear();
}

void BossAttackTriggerManager::Update() {
	for (auto& trigger : bossAttackTriggers_) {
		trigger->Update();
	}
}

void BossAttackTriggerManager::Create(const BossAttackTrigger::Desc desc) {
	BossAttackTrigger* bossAttackTrigger = new BossAttackTrigger();
	bossAttackTrigger->SetBoss(boss_);
	bossAttackTrigger->Initialize(desc);
	bossAttackTriggers_.emplace_back(std::move(bossAttackTrigger));
}

void BossAttackTriggerManager::Delete(BossAttackTrigger* bossAttackTrigger) {
	// bossAttackTriggers_ がリスト内に存在するかを確認し、存在する場合はそのイテレータを取得する
	auto it = std::find_if(bossAttackTriggers_.begin(), bossAttackTriggers_.end(), [bossAttackTrigger](const auto& ptr) {
		return ptr.get() == bossAttackTrigger;
		});

	// block が見つかった場合は削除する
	if (it != bossAttackTriggers_.end()) {
		bossAttackTriggers_.erase(it);
	}
}

void BossAttackTriggerManager::Clear() {
	bossAttackTriggers_.clear();
}

void BossAttackTriggerManager::LoadJson(uint32_t stageIndex) {
	const std::filesystem::path kDirectoryPath = "Resources/Data/Boss/BossTrigger/" + std::to_string(stageIndex);
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
	nlohmann::json::iterator itGroup = root.find("Pendulum");
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
				if (objectName.find("Trigger") != std::string::npos) {
					BossAttackTrigger::Desc desc{};
					for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {
						//アイテム名を取得
						const std::string& itemNameObject = itItemObject.key();
						if (itemNameObject == "pos") {
							desc.pos = itItemObject->get<float>();
						}
						else if (itemNameObject == "state") {
							switch (itItemObject->get<int>()) {
							case 0:
								desc.state = BossStateManager::kRoot;
								break;
							case 1:
								desc.state = BossStateManager::kHook;
								break;
							default:
								break;
							}
						}
						Create(desc);
					}
				}
			}
		}
	}
}
