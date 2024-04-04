#pragma once
#include "Scene/BaseScene.h"

#include <memory>
#include <vector>

#include "Graphics/Model.h"
#include "Graphics/LightManager.h"
#include "Math/Camera.h"
#include "Math/Transform.h"
#include "Math/Random.h"

#include "Block/BlockManager.h"
#include "CameraManager/CameraManager.h"
#include "DebugCamera.h"
#include "Editor/EditorManager.h"
#include "Floor/floor.h"
#include "Player/Player.h"

class GameScene :
    public BaseScene {
public:

    void OnInitialize() override;
    void OnUpdate() override;
    void OnFinalize() override;

private:
    std::shared_ptr<CameraManager> cameraManager_;
    std::shared_ptr<DirectionalLight> directionalLight_;
    
    std::unique_ptr<BlockManager> blockManager_;
    std::unique_ptr<EditorManager> editorManager_;

    std::unique_ptr<Player> player_;
    std::array<std::unique_ptr<Floor>,2> floor_;

};
