#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Math/MathUtils.h"
#include "Math/Transform.h"
#include "Math/Camera.h"

class Player;
class StageCamera :
    public GameObject {
public:
    void Initialize();
    void Update();

    const std::shared_ptr<Camera>& GetCamera() const { return camera_; }
    void SetPlayer(Player* player) { player_ = player; }
    void SetRenderManager();

private:
    std::shared_ptr<Camera> camera_;
    Player* player_;
    Vector3 offset_;
    Vector3 eulerAngle_;
};