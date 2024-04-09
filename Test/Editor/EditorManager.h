#pragma once

#include "Editor/BlockEditor/BlockEditor.h"
#include "Editor/FireBarEditor/FireBarEditor.h"
#include "Editor/FloorEditor/FloorEditor.h"
#include "Editor/PendulumEditor/PendulumEditor.h"

class BlockManager;
class FireBarManager;
class FloorManager;
class PendulumManager;
class EditorManager {
public:
	void Initialize(BlockManager* blockEditor, FireBarManager* fireBarEditor, FloorManager* floorEditor, PendulumManager* pendulumManager);

	void Update();

private:
	int stageIndex_;

	std::unique_ptr<BlockEditor> blockEditor_;
	std::unique_ptr<FireBarEditor> fireBarEditor_;
	std::unique_ptr<FloorEditor> floorEditor_;
	std::unique_ptr<PendulumEditor> pendulumEditor_;
};