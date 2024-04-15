#include "StageBlockManager.h"

#include "CollisionAttribute.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"

void StageBlockManager::Initialize() {
	
}

void StageBlockManager::Update() {
	UpdateTransform();
}

void StageBlockManager::Appear()
{

}

void StageBlockManager::UpdateTransform() {
	transform.UpdateMatrix();
}

