#include "BulletManager.h"

#include "CharacterState.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"

void BulletManager::Initialize() {
	for (auto& bullet : bullets_) {
		bullet = std::make_unique<PlayerBullet>();
		bullet->Initialize();
	}
	isCreateBullet_ = false;
	bulletCount_ = 0;
#pragma region Json
	JSON_OPEN("Resources/Data/Player/PlayerBulletManager.json");
	JSON_OBJECT("PlayerBulletManager");
	JSON_LOAD(interval_);
	JSON_LOAD(velocity_);
	JSON_CLOSE();
#pragma endregion
}

void BulletManager::Update(const Vector3& pos) {
	if (characterState_==Character::State::kChase &&
		canCreateBullet_ &&
		(Input::GetInstance()->IsKeyPressed(DIK_LSHIFT) || (Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_B))) {
		isCreateBullet_ = true;
		canCreateBullet_ = false;
		bulletCount_ = 0;
		time_ = 0;
	}
	if (isCreateBullet_) {
		Create(pos);
	}
	bool allDead = true;
	for (auto& bullet : bullets_) {
		bullet->Update();
		if (bullet->GetIsAlive()) {
			allDead = false;
		}
	}

	if (allDead) {
		canCreateBullet_ = true;
	}
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Player")) {
		if (ImGui::TreeNode("PlayerBulletManager")) {
			ImGui::DragFloat3("velocity_", &velocity_.x, 0.1f);
			int i = interval_;
			ImGui::DragInt("interval", &i);
			interval_ = i;
			ImGui::Checkbox("canCreateBullet_", &canCreateBullet_);
			ImGui::Checkbox("isCreateBullet_", &isCreateBullet_);
			if (ImGui::Button("Save")) {
				JSON_OPEN("Resources/Data/Player/PlayerBulletManager.json");
				JSON_OBJECT("PlayerBulletManager");
				JSON_SAVE(interval_);
				JSON_SAVE(velocity_);
				JSON_CLOSE();
			}
			ImGui::TreePop();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG

}

void BulletManager::Create(const Vector3& pos) {
	if (time_ <= 0 &&
		bulletCount_ <= kMaxBullet) {
		time_ = interval_;
		bullets_.at(bulletCount_)->Create(pos, velocity_);
		bulletCount_++;
		if (bulletCount_ >= kMaxBullet) {
			isCreateBullet_ = false;
		}
	}
	time_--;
}

void BulletManager::Reset() {
	for (auto& bullet : bullets_) {
		bullet->Reset();
	}
}
