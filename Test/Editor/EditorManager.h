#pragma once

#include "Editor/BlockEditor/BlockEditor.h"

class BlockManager;
class EditorManager {
public:
	void Initialize(BlockManager* blockEditor);

	void Update();

private:
	int stageIndex_;

	std::unique_ptr<BlockEditor> blockEditor_;
};