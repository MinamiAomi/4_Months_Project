#include "Test.h"

#include <memory>

#include "Scene/SceneManager.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/Model.h"
#include "Graphics/Sprite.h"

#include "GameScene.h"

void Test::OnInitialize() {
	SceneManager* sceneManager = SceneManager::GetInstance();
	//シーン設定
	sceneManager->ChangeScene<GameScene>(false);

	ResourceManager* resourceManager = ResourceManager::GetInstance();
	resourceManager->AddTexture("circle", Texture::Load("Resources/whiteCircle.png"));
	resourceManager->AddTexture("white2x2", Texture::Load("Resources/white2x2.png"));
	resourceManager->AddTexture("hp", Texture::Load("Resources/playerHP.png"));
	resourceManager->AddModel("boss",   Model::Load("Resources/Boss/boss.gltf"));
	resourceManager->AddModel("floor", Model::Load("Resources/Floor/floor.gltf"));
	resourceManager->AddModel("player", Model::Load("Resources/Player/player.obj"));
	resourceManager->AddModel("block", Model::Load("Resources/Block/Block.gltf"));
	resourceManager->AddModel("skydome", Model::Load("Resources/Skydome/skydome.gltf"));
}

void Test::OnFinalize() {
}
