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
#pragma region Json
	JSON_OPEN("Resources/Data/Player/PlayerBulletManager.json");
	JSON_OBJECT("PlayerBulletManager");
	JSON_LOAD(velocity_);
	JSON_LOAD(coolTime_);
	JSON_CLOSE();
#pragma endregion
}

void BulletManager::Update(const Vector3& pos) {
	if (time_ > 0) {
		time_--;
	}
	if (time_ == 0 &&
		characterState_ == Character::kChase &&
		(Input::GetInstance()->IsKeyPressed(DIK_LSHIFT) || (Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A))) {
		Create(pos);
		time_ = coolTime_;
	}
	for (auto& bullet : bullets_) {
		bullet->Update();
	}
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Player")) {
		if (ImGui::TreeNode("PlayerBulletManager")) {
			int time = time_;
			ImGui::DragInt("time_", &time);
			time_ = time;
			int coolTime = coolTime_;
			ImGui::DragInt("coolTime_", &coolTime);
			coolTime_ = coolTime;
			ImGui::DragFloat3("velocity_", &velocity_.x, 0.1f);

			if (ImGui::Button("Save")) {
				JSON_OPEN("Resources/Data/Player/PlayerBulletManager.json");
				JSON_OBJECT("PlayerBulletManager");
				JSON_SAVE(coolTime_);
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
	for (auto& bullet : bullets_) {
		if (!bullet->GetIsAlive()) {
			bullet->Create(pos, velocity_);
			break;
		}
	}
}

void BulletManager::Reset() {
	for (auto& bullet : bullets_) {
		bullet->Reset();
	}
}
