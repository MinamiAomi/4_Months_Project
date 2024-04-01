#pragma once
#include "Scene/BaseScene.h"

#include <memory>
#include <vector>

#include "Math/Camera.h"
#include "Math/Transform.h"
#include "Math/Random.h"
#include "Graphics/Model.h"
#include "Graphics/LightManager.h"

#include "Block/BlockManager.h"
#include "Editor/EditorManager.h"
#include "DebugCamera.h"

class GameScene :
    public BaseScene {
public:

    void OnInitialize() override;
    void OnUpdate() override;
    void OnFinalize() override;

private:
    std::shared_ptr<DebugCamera> debugCamera_;
    std::shared_ptr<DirectionalLight> directionalLight_;
    
    std::unique_ptr<BlockManager> blockManager_;
    std::unique_ptr<EditorManager> editorManager_;
};
