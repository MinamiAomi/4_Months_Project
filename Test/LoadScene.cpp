#include "LoadScene.h"

#include <cassert>

#include "Graphics/GameWindow.h"
#include "TitleScene.h"
#include "Test.h"
#include "Scene/SceneManager.h"
#include "TitleScene.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"

namespace {
    const char* kMemberPaths[] = {
        "Resources/Load/rikihisa.png",
        "Resources/Load/aomi.png",
        "Resources/Load/mint.png",
        "Resources/Load/fuga.png",
        "Resources/Load/sakiren.png",
    };

    const char* kDonburiPath = "Resources/Load/donburi.png";
    const char* kLogoPath = "Resources/Load/logo.png";

    struct SpriteParameter {
        Vector2 offset;
        uint8_t drawOrder;
    };

    SpriteParameter kMemberPrameters[] = {
        { {  120.0f, -55.0f }, 1 },
        { {   60.0f, -55.0f }, 1 },
        { {    0.0f, -55.0f }, 1 },
        { {  -60.0f, -55.0f }, 1 },
        { { -120.0f, -55.0f }, 1 },
    };

    SpriteParameter kDonburiParameter = {
        { 0.0f, 0.0f, }, 0
    };
    SpriteParameter kLogoParameter = {
       { 0.0f, 136.0f, }, 0
    };
}

void LoadScene::OnInitialize() {
    assert(test_);

    Vector2 center = { 1920.0f * 0.5f, 1080.0f * 0.5f };
    for (int i = 0; i < NumMembers; ++i) {
        auto texture = Texture::Load(kMemberPaths[i]);
        texture->SetExtension(Texture::Extension::Clamp);
        texture->SetInterpolation(Texture::Interpolation::Point);
        members_[i] = std::make_unique<Sprite>();
        members_[i]->SetTexture(texture);
        members_[i]->SetPosition(center - kMemberPrameters[i].offset);
        members_[i]->SetScale({ texture->GetWidth(), texture->GetHeight() });
        members_[i]->SetDrawOrder(kMemberPrameters[i].drawOrder);
    }

    auto texture = Texture::Load(kDonburiPath);
    texture->SetExtension(Texture::Extension::Clamp);
    texture->SetInterpolation(Texture::Interpolation::Point);
    donburi_ = std::make_unique<Sprite>();
    donburi_->SetTexture(texture);
    donburi_->SetPosition(center - kDonburiParameter.offset);
    donburi_->SetScale({ texture->GetWidth(), texture->GetHeight() });
    donburi_->SetDrawOrder(0);

    texture = Texture::Load(kLogoPath);
    texture->SetExtension(Texture::Extension::Clamp);
    texture->SetInterpolation(Texture::Interpolation::Point);
    logo_ = std::make_unique<Sprite>();
    logo_->SetTexture(texture);
    logo_->SetPosition(center - kLogoParameter.offset);
    logo_->SetScale({ texture->GetWidth(), texture->GetHeight() });
    logo_->SetDrawOrder(0);
}

void LoadScene::OnUpdate() {
    const float kAnimationFrame = 300.0f;

    const float kHeight = 55.0f;
    const float kInStart = 0.1f;
    const float kJumpStart = 0.7f;
    const float kJumpEnd = 0.9f;

    auto Easing = [](float t) {
        return 1.0f - std::cos((t * Math::Pi) / 2.0f);
        };

    animationTime_ += 1.0f / kAnimationFrame;
    animationTime_ = std::fmod(animationTime_, 1.0f);

    Vector2 center = { 1920.0f * 0.5f, 1080.0f * 0.5f };

    if (animationTime_ >= kInStart && animationTime_ < kJumpStart) {
        float t = (animationTime_ - kInStart) / (kJumpStart - kInStart);
        float a = 1.0f / float(NumMembers);
        for (int i = 0; i < NumMembers; ++i) {
            float b = (t - (a * i)) / a;
            Vector2 offset = { 0.0f,  kHeight * std::clamp(b, 0.0f, 1.0f) };
            members_[i]->SetPosition(center - kMemberPrameters[i].offset + offset);
        }
    }
    if (animationTime_ >= kJumpStart && animationTime_ < kJumpEnd) {
        float t = (animationTime_ - kJumpStart) / (kJumpEnd - kJumpStart);
        for (int i = 0; i < NumMembers; ++i) {
            Vector2 offset = { 0.0f, std::sin(Math::Pi * t) * 25.0f + kHeight };
            members_[i]->SetPosition(center - kMemberPrameters[i].offset + offset);
        }
    }
    if(animationTime_ >= kJumpEnd && animationTime_ < 1.0f) {
        float t = (animationTime_ - kJumpEnd) / (1.0f - kJumpEnd);
        for (int i = 0; i < NumMembers; ++i) {
            Vector2 offset = { 0.0f, kHeight * std::clamp(1.0f - t, 0.0f, 1.0f) };
            members_[i]->SetPosition(center - kMemberPrameters[i].offset + offset);
        }
    }


#ifdef ENABLE_IMGUI
    ImGui::SliderFloat("AnimationTime", &animationTime_, 0.0f, 1.0f);
#endif ENABLE_IMGUI


    if (test_->ResourcesLoaded()) {
        SceneManager::GetInstance()->ChangeScene<TitleScene>(true);
    }
}

void LoadScene::OnFinalize() {
}
