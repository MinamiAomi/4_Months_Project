#include "Sprite.h"

#include "Core/SamplerManager.h"
#include "GameWindow.h"

std::shared_ptr<Texture> Texture::Load(const std::filesystem::path& path) {
    // privateコンストラクタをmake_sharedで呼ぶためのヘルパー
    struct Helper : Texture {
        Helper() : Texture() {}
    };
    std::shared_ptr<Texture> texture = std::make_shared<Helper>();
    texture->resource_.CreateFromWICFile(path);
    return texture;
}



const DescriptorHandle& Texture::GetSampler() const {
    switch (interpolation_) {
    case Texture::Interpolation::Linear:
        switch (extension_) {
        case Texture::Extension::Wrap:
            return SamplerManager::LinearWrap;
        case Texture::Extension::Clamp:
            return SamplerManager::LinearClamp;
        }
        break;
    case Texture::Interpolation::Point:
        switch (extension_) {
        case Texture::Extension::Wrap:
            return SamplerManager::PointWrap;
        case Texture::Extension::Clamp:
            return SamplerManager::PointClamp;
        }
        break;
    }
    return SamplerManager::AnisotropicWrap;
}

std::list<Sprite*> Sprite::instanceList_;

Sprite::Sprite() {
    instanceList_.emplace_back(this);
}

Sprite::~Sprite() {
    instanceList_.remove(this);
}

void Sprite::SetTexture(const std::shared_ptr<Texture>& texture) {
    texture_ = texture;
    if (texture_) { return; }
    texcoordBase_ = {};
    texcoordSize_ = { texture_->GetWidth(), texture_->GetHeight() };
}

void Sprite::SetPosition(const Vector2& position)
{
    GameWindow* w = GameWindow::GetInstance();
    float y = position.y / w->GetClientHeight();
    float x = position.x / w->GetClientWidth();
    RECT rect;
    GetWindowRect(GameWindow::GetInstance()->GetHWND(), &rect);
    float width = float(rect.right - rect.left);
    float height = float(rect.bottom - rect.top);
    position_ = { width * x,height * y };
}

void Sprite::SetScale(const Vector2& scale)
{
    GameWindow* w = GameWindow::GetInstance();
    RECT rect;
    GetWindowRect(GameWindow::GetInstance()->GetHWND(), &rect);
    float width = float(rect.right - rect.left);
    float height = float(rect.bottom - rect.top);

    float x = width / w->GetClientWidth();
    float y = height / w->GetClientHeight();
    scale_ = Vector2{ scale.x * x,scale.y * y };
}
