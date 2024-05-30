#include "LoadScene.h"

#include <cassert>

#include "Graphics/GameWindow.h"
#include "TitleScene.h"
#include "Test.h"
#include "Scene/SceneManager.h"
#include "TitleScene.h"

void LoadScene::OnInitialize() {
    assert(test_);
 
    auto texture = Texture::Load("Resources/NowLoading.png");
    load_ = std::make_unique<Sprite>();
    load_->SetTexture(texture);
    load_->SetTexcoordRect({}, { 480.0f, 270.0f });
    load_->SetPosition({});
    load_->SetScale({ float(GameWindow::GetInstance()->GetClientWidth()), float(GameWindow::GetInstance()->GetClientHeight()) });
    load_->SetAnchor({});
}

void LoadScene::OnUpdate() {
    animationTime_ += 1.0f / (11.0f * 5.0f);
    animationTime_ = std::fmod(animationTime_, 1.0f);
    load_->SetTexcoordBase({ 480.0f * int(11.0f * animationTime_), 0.0f });

    if (test_->ResourcesLoaded()) {
        SceneManager::GetInstance()->ChangeScene<TitleScene>(true);
    }
}

void LoadScene::OnFinalize() {
}
