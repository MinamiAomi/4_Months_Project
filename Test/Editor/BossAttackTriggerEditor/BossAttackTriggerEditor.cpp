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
	desc.desc.transform.translate.z = -50.0f;
	desc.desc.collider = StageGimmick::Collider{};
	desc.desc.collider->size = { 2.0f,2.0f,2.0f };
	desc.state = BossStateManager::State::kHook;
	bossAttackTrigger_->SetCamera(camera_);
	bossAttackTrigger_->SetBoss(boss_);
	bossAttackTrigger_->Initialize(desc);
	isAlive_ = false;
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
		/*if (ImGui::TreeNode("CreateBossAttackTrigger")) {
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
		}*/
		for (uint32_t i = 0; auto & bossAtackTrigger : bossAttackTriggerManager_->GetBossAttackTriggers()) {
			if (bossAtackTrigger.get() == nullptr) {
				continue;
			}
			if (ImGui::TreeNode(("BossState:" + std::to_string(i)).c_str())) {
				auto& desc = bossAtackTrigger->GetDesc();
				ImGui::DragFloat(("pos:" + std::to_string(i)).c_str(), &desc.desc.transform.translate.x, 1.0f);
				const char* items[] = { "Root", "Hook" ,"LowerAttack","InsideAttack" "BeamAttack"};
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
					case BossStateManager::State::kLowerAttack:
					{
						desc.state = BossStateManager::State::kLowerAttack;
					}
					break;
					case BossStateManager::State::kInsideAttack:
					{
						desc.state = BossStateManager::State::kInsideAttack;
					}
					break;
					case BossStateManager::State::kBeamAttack:
					{
						desc.state = BossStateManager::State::kBeamAttack;
					}
					break;
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
	stageName;
	//nlohmann::json root;

	//root = nlohmann::json::object();

	//root[fileName_] = nlohmann::json::object();

	//for (size_t i = 0; auto & bossAttackTrigger : bossAttackTriggerManager_->GetBossAttackTriggers()) {
	//	auto desc = bossAttackTrigger->GetDesc();
	//	root[fileName_]["objectData"][("BossAttackTrigger:" + std::to_string(i)).c_str()]["position"] = desc.pos;
	//	root[fileName_]["objectData"][("BossAttackTrigger:" + std::to_string(i)).c_str()]["state"] = static_cast<int>(desc.state);
	//	i++;
	//}

	//const std::filesystem::path kDirectoryPath = "Resources/Data/" + fileName_ + "/" + std::to_string(stageName);

	//if (!std::filesystem::exists(kDirectoryPath)) {
	//	std::filesystem::create_directories(kDirectoryPath);
	//}
	//std::string filePath = kDirectoryPath.string() + std::string(".json");

	//std::ofstream file(filePath);

	//if (file.fail()) {
	//	assert(!"fileSaveFailed");
	//	return;
	//}

	//file << std::setw(4) << root << std::endl;

	//file.close();
}

//void BossAttackTriggerEditor::LoadFile(uint32_t stageName) {
//	bossAttackTriggerManager_->LoadJson(stageName);
//}

void BossAttackTriggerEditor::Clear() {
	bossAttackTriggerManager_->Clear();
}
