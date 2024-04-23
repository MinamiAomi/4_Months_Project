#include "Test.h"

#include <memory>
#include <fstream>

#include "Externals/nlohmann/json.hpp"

#include "Scene/SceneManager.h"
#include "Framework/ResourceManager.h"
#include "Graphics/Model.h"
#include "Graphics/Sprite.h"
#include "Audio/Sound.h"

#include "GameScene.h"

static const char ResourceAssociationFile[] = "Resources/Association.json";

void Test::OnInitialize() {
    SceneManager* sceneManager = SceneManager::GetInstance();
    //シーン設定
    sceneManager->ChangeScene<GameScene>(false);

    LoadResources();
}

void Test::OnFinalize() {}

void Test::LoadResources() {
    // ファイルを開く
    std::ifstream file;
    file.open(ResourceAssociationFile);
    // 開けなかった
    assert(file.is_open());
    
    nlohmann::json json;
    file >> json;
    file.close();

    ResourceManager* resourceManager = ResourceManager::GetInstance();
    for (auto& texture : json["Texture"].items()) {
        resourceManager->AddTexture(texture.key(), Texture::Load("Resources/" + texture.value().get<std::string>()));
    }
    for (auto& model : json["Model"].items()) {
        resourceManager->AddModel(model.key(), Model::Load("Resources/" + model.value().get<std::string>()));
    }
    for (auto& sound : json["Sound"].items()) {
        resourceManager->AddSound(sound.key(), Sound::Load("Resources/" + sound.value().get<std::string>()));
    }
}
