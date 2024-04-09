#pragma once

#include "Editor/BlockEditor/BlockEditor.h"
#include "Editor/FireBarEditor/FireBarEditor.h"

class BlockManager;
class FireBarManager;
class EditorManager {
public:
	void Initialize(BlockManager* blockEditor, FireBarManager* fireBarEditor);

	void Update();

private:
	int stageIndex_;

	std::unique_ptr<BlockEditor> blockEditor_;
	std::unique_ptr<FireBarEditor> fireBarEditor_;
};