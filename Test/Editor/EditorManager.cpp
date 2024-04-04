#include "EditorManager.h"

#include "Block/BlockManager.h"
#include "Externals/ImGui/imgui.h"

void EditorManager::Initialize(BlockManager* blockEditor) {
	blockEditor_ = std::make_unique<BlockEditor>();

	blockEditor_->SetBlockManager(blockEditor);

	blockEditor_->Initialize();

	stageIndex_ = 0;
}

void EditorManager::Update() {
	static const uint32_t kCount = 10;
	ImGui::Begin("StageEditor");
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
		if (ImGui::Button("Load") ) {
			blockEditor_->Clear();
			blockEditor_->LoadFile(stageIndex_);
		}
		ImGui::TreePop();
	}
	ImGui::End();
	blockEditor_->Update();
}
