#include "SkyBlockManager.h"

#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"
#include "./Test/CharacterState.h"
#include "./File/JsonHelper.h"

#include "SkyBlock.h"



void SkyBlockManager::Initialize() {

#pragma region パラメーター
	JSON_OPEN("Resources/Data/SkyBlock/SkyBlock.json");
	JSON_OBJECT("SkyBlock");
	JSON_LOAD(appearLength);
	JSON_CLOSE();
#pragma endregion

	if (boss_) {
		for (int i = 0;  std::unique_ptr<SkyBlock>&skyblock : skyBlocks_) {
			float scale = rng_.NextFloatRange(1.0f, 3.0f);
			float x = 30.0f + rng_.NextFloatRange(0.0f, 10.0f);
			float y = 3.0f + rng_.NextFloatRange(0.0f, 7.0f);
			float z = boss_->transform.translate.z + rng_.NextFloatRange(-appearLength , appearLength );
			skyblock = std::make_unique<SkyBlock>();
			skyblock->SetBoss(boss_);
			
			if (i % 2 == 0) {
				skyblock->Initialize({ x,y,z }, { x,y,z }, { scale ,scale ,scale });
			}
			else {
				skyblock->Initialize({ -x,y,z }, { x,y,z },{ scale ,scale ,scale });
			}
			i++;
		}
	}
}

void SkyBlockManager::Update() {
	for (const std::unique_ptr<SkyBlock>& skyBlock : skyBlocks_) {
		skyBlock->Update(appearLength);
	}
	DebugParam();
	DeadCheck();
}

void SkyBlockManager::Reset()
{
	Initialize();
}

void SkyBlockManager::Appear(SkyBlock& skyBlock)
{
	if (boss_) {
		float scale = rng_.NextFloatRange(1.0f, 3.0f);
		float x;
		if (skyBlock.transform.translate.x > 0) {
			x = 30.0f + rng_.NextFloatRange(0.0f, 10.0f);
		}
		else {
			x = -30.0f + -rng_.NextFloatRange(0.0f, 10.0f);
		}
		float y = 3.0f + rng_.NextFloatRange(0.0f, 7.0f);

		if (characterState_ && Character::kRunAway) {
			skyBlock.Initialize({ x,y ,boss_->transform.worldMatrix.GetTranslate().z - appearLength}, { x,y ,boss_->transform.worldMatrix.GetTranslate().z - appearLength }
			, { scale ,scale ,scale });
			
		}
		else {			
			skyBlock.Initialize({ x, y ,boss_->transform.worldMatrix.GetTranslate().z + appearLength }, { x,y ,boss_->transform.worldMatrix.GetTranslate().z - appearLength }
			, { scale ,scale ,scale });
			
		}
	}


}

void SkyBlockManager::DeadCheck()
{
	for (const std::unique_ptr<SkyBlock>& skyblock : skyBlocks_) {
		if (skyblock->GetIsDead()) {
			Appear(*skyblock.get());
		}
	}
}

void SkyBlockManager::DebugParam()
{
#ifdef _DEBUG
		ImGui::Begin("Editor");
		if (ImGui::BeginMenu("SkyBlock")) {
			ImGui::DragFloat("appearLength", &appearLength);
			if (ImGui::Button("Save")) {
				JSON_OPEN("Resources/Data/SkyBlock/SkyBlock.json");
				JSON_OBJECT("SkyBlock");
				JSON_SAVE(appearLength);
				JSON_CLOSE();
			}
			ImGui::EndMenu();
		}
		ImGui::End();
#endif // _DEBUG
}


void SkyBlockManager::UpdateTransform() {
	transform.UpdateMatrix();
}

