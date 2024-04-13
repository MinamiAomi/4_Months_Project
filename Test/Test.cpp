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
	
	resourceManager->AddTexture("HPBar_Gauge", Texture::Load("Resources/Player/PlayerUI/HPBar_Gauge.png"));
	resourceManager->AddTexture("HPBar_Flame", Texture::Load("Resources/Player/PlayerUI/HPBar_Flame.png"));
	resourceManager->AddTexture("RevengeBar_RevengeGauge", Texture::Load("Resources/Player/PlayerUI/RevengeBar_RevengeGauge.png"));
	resourceManager->AddTexture("RevengeBar_Flame", Texture::Load("Resources/Player/PlayerUI/RevengeBar_Flame.png"));
	resourceManager->AddTexture("RevengeBar_WeaponGauge", Texture::Load("Resources/Player/PlayerUI/RevengeBar_WeaponGauge.png"));

	resourceManager->AddModel("bossBody", Model::Load("Resources/Boss/Body/boss.gltf"));
	resourceManager->AddModel("bossRightArm", Model::Load("Resources/Boss/RightArm/rightArm.gltf"));
	resourceManager->AddModel("bossLeftArm", Model::Load("Resources/Boss/LeftArm/leftArm.gltf"));
	resourceManager->AddModel("floor", Model::Load("Resources/Floor/floor.gltf"));
	resourceManager->AddModel("player", Model::Load("Resources/Player/player.gltf"));
	resourceManager->AddModel("block", Model::Load("Resources/Block/Block.gltf"));
	resourceManager->AddModel("skydome", Model::Load("Resources/Skydome/skydome.gltf"));
	resourceManager->AddModel("fireBarBar", Model::Load("Resources/FireBar/Bar/bar.gltf"));
	resourceManager->AddModel("fireBarCenter", Model::Load("Resources/FireBar/Center/center.gltf"));
}

void Test::OnFinalize() {}
