#include "BossAttackTriggerEditor.h"

#include <Windows.h>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <list>
#include <vector>

#include "Externals/nlohmann/json.hpp"
#include "Framework/ResourceManager.h"
#include "Externals/ImGui/imgui.h"

void BossAttackTriggerEditor::Initialize() {

	fileName_ = "BossAttackTrigger";

	bossAttackTrigger_ = std::make_unique<BossAttackTrigger>();
	BossAttackTrigger::Desc desc{};
	desc.pos = -50.0f;
	desc.state = BossStateManager::State::kHook;
	bossAttackTrigger_->SetCamera(camera_);
	bossAttackTrigger_->SetBoss(boss_);
	bossAttackTrigger_->Initialize(desc);
	isAlive_ = true;
#ifdef _DEBUG
	bossAttackTrigger_->SetIsColliderAlive(false);
#else
	bossAttackTrigger_->SetIsAlive(false);
#endif // _DEBUG

}

void BossAttackTriggerEditor::Update() {
#ifdef _DEBUG
	static bool isPlay = false;
	ImGui::Begin("StageEditor");
	if (ImGui::TreeNode("BossAttackEditor")) {
		if (ImGui::Checkbox("ModelIsAlize", &isAlive_)) {
			bossAttackTriggerManager_->SetModelIsAlive(isAlive_);
		}
		if (ImGui::TreeNode("CreateBossAttackTrigger")) {
			bossAttackTrigger_->SetIsAlive(true);
			auto& desc = bossAttackTrigger_->GetDesc();
			ImGui::DragFloat("position", &desc.pos, 0.1f);
			const char* items[] = { "Root", "Hook","FloorAll","LongDistanceAttack" };
			static int selectedItem = static_cast<int>(desc.state);
			if (ImGui::Combo("State", &selectedItem, items, IM_ARRAYSIZE(items))) {
				desc.state = static_cast<BossStateManager::State>(selectedItem);
				switch (desc.state) {
				case BossStateManager::State::kRoot:
				{
					desc.state = BossStateManager::State::kRoot;
				}
				break;
				case BossStateManager::State::kHook:
				{
					desc.state = BossStateManager::State::kHook;
				}
				break;
				case BossStateManager::State::kFloorAll:
				{
					desc.state = BossStateManager::State::kFloorAll;
				}
				break;
				case BossStateManager::State::kLongDistanceAttack:
				{
					desc.state = BossStateManager::State::kLongDistanceAttack;
				}
				break;
				}
			}
			bossAttackTrigger_->SetDesc(desc);
			if (ImGui::Button("Create")) {
				bossAttackTriggerManager_->Create(desc);
			}

			ImGui::TreePop();
			isCreate_ = true;
		}
		else {
			bossAttackTrigger_->SetIsAlive(false);
		}
		for (uint32_t i = 0; auto & bossAtackTrigger : bossAttackTriggerManager_->GetBossAttackTriggers()) {
			if (bossAtackTrigger.get() == nullptr) {
				continue;
			}
			if (ImGui::TreeNode(("BossState:" + std::to_string(i)).c_str())) {
				auto& desc = bossAtackTrigger->GetDesc();
				ImGui::DragFloat(("pos:" + std::to_string(i)).c_str(), &desc.pos, 1.0f);
				const char* items[] = { "Root", "Hook" ,"FloorAll","LongDistanceAttack" };
				int selectedItem = static_cast<int>(desc.state);
				if (ImGui::Combo(("State:" + std::to_string(i)).c_str(), &selectedItem, items, IM_ARRAYSIZE(items))) {
					desc.state = static_cast<BossStateManager::State>(selectedItem);
					switch (desc.state) {
					case BossStateManager::State::kRoot:
					{
						desc.state = BossStateManager::State::kRoot;
					}
					break;
					case BossStateManager::State::kHook:
					{
						desc.state = BossStateManager::State::kHook;
					}
					break;
					case BossStateManager::State::kFloorAll:
					{
						desc.state = BossStateManager::State::kFloorAll;
					}
					break;
					case BossStateManager::State::kLongDistanceAttack:
					{
						desc.state = BossStateManager::State::kLongDistanceAttack;
					}
					}
				}
				bossAtackTrigger->SetDesc(desc);
				if (ImGui::Button("Delete")) {
					bossAttackTriggerManager_->Delete(bossAtackTrigger.get());
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
		bossAttackTrigger_->SetIsAlive(false);
	}
	ImGui::End();
	bossAttackTrigger_->Update();
#endif // _DEBUG
}

void BossAttackTriggerEditor::SaveFile(uint32_t stageName) {
	nlohmann::json root;

	root = nlohmann::json::object();

	root[fileName_] = nlohmann::json::object();

	for (size_t i = 0; auto & bossAttackTrigger : bossAttackTriggerManager_->GetBossAttackTriggers()) {
		auto desc = bossAttackTrigger->GetDesc();
		root[fileName_]["objectData"][("BossAttackTrigger:" + std::to_string(i)).c_str()]["position"] = desc.pos;
		root[fileName_]["objectData"][("BossAttackTrigger:" + std::to_string(i)).c_str()]["state"] = static_cast<int>(desc.state);
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

void BossAttackTriggerEditor::LoadFile(uint32_t stageName) {
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

				BossAttackTrigger::Desc desc{};
				for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {
					//アイテム名を取得
					const std::string& itemNameObject = itItemObject.key();
					if (itemNameObject == "position") {
						desc.pos = itItemObject->get<float>();
					}
					else if (itemNameObject == "state") {
						desc.state = static_cast<BossStateManager::State>(itItemObject->get<int>());
					}

				}
				bossAttackTriggerManager_->Create(desc);
			}
		}
	}
}

void BossAttackTriggerEditor::Clear() {
	bossAttackTriggerManager_->Clear();
}
