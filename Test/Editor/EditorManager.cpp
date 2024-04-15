#include "EditorManager.h"

#include "StageGimmick/Block/BlockManager.h"
#include "StageGimmick/FireBar/FireBarManager.h"
#include "StageGimmick/Floor/FloorManager.h"
#include "Externals/ImGui/imgui.h"

void EditorManager::Initialize(BlockManager* blockEditor, FireBarManager* fireBarEditor, FloorManager* floorEditor, PendulumManager* pendulumManager) {
	blockEditor_ = std::make_unique<BlockEditor>();
	fireBarEditor_ = std::make_unique<FireBarEditor>();
	floorEditor_ = std::make_unique<FloorEditor>();
	pendulumEditor_ = std::make_unique<PendulumEditor>();

	blockEditor_->SetBlockManager(blockEditor);
	fireBarEditor_->SetFireManager(fireBarEditor);
	floorEditor_->SetFloorManager(floorEditor);
	pendulumEditor_->SetPendulumManager(pendulumManager);

	blockEditor_->Initialize();
	fireBarEditor_->SetPlayer(player_);
	fireBarEditor_->Initialize();
	floorEditor_->Initialize();
	pendulumEditor_->SetPlayer(player_);
	pendulumEditor_->Initialize();

	stageIndex_ = 0;
}

void EditorManager::Update() {
	static const uint32_t kCount = 10;
#ifdef _DEBUG


	ImGui::Begin("StageEditorSave");
	if (ImGui::TreeNode("SaveFile")) {
		std::list<std::string> stageList;
		for (uint32_t i = 0; i < kCount; i++) {
			stageList.emplace_back(std::to_string(i));
		}

		// std::vector に変換する
		std::vector<const char*> stageArray;
		for (const auto& stage : stageList) {
			stageArray.push_back(stage.c_str());
		}
		// Combo を使用する
		if (ImGui::Combo("Stage", &stageIndex_, stageArray.data(), static_cast<int>(stageArray.size()))) {
		}
		if (ImGui::Button("Save")) {
			blockEditor_->SaveFile(stageIndex_);
			fireBarEditor_->SaveFile(stageIndex_);
			floorEditor_->SaveFile(stageIndex_);
			pendulumEditor_->SaveFile(stageIndex_);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("LoadFile")) {
		std::list<std::string> stageList;
		for (uint32_t i = 0; i < kCount; i++) {
			stageList.emplace_back(std::to_string(i));
		}

		// std::vector に変換する
		std::vector<const char*> stageArray;
		for (const auto& stage : stageList) {
			stageArray.push_back(stage.c_str());
		}

		// Combo を使用する
		if (ImGui::Combo("Stage", &stageIndex_, stageArray.data(), static_cast<int>(stageArray.size()))) {

		}
		if (ImGui::Button("Load")) {
			blockEditor_->Clear();
			fireBarEditor_->Clear();
			blockEditor_->LoadFile(stageIndex_);
			fireBarEditor_->LoadFile(stageIndex_);
			floorEditor_->LoadFile(stageIndex_);
			pendulumEditor_->LoadFile(stageIndex_);
		}
		ImGui::TreePop();
	}
	ImGui::End();
#endif // _DEBUG
	pendulumEditor_->Update();
	blockEditor_->Update();
	fireBarEditor_->Update();
	floorEditor_->Update();
}
