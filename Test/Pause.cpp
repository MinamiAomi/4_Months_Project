#include "Pause.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"

bool Pause::isPause_ = false;

void Pause::Initialize() {
	LoadJson();
	ToTitle_ = CreateSprite(ToTitleData_, "toTitle");
	retry_ = CreateSprite(retryData_, "retry");
	grey_ = CreateSprite(greyData_, "grey2x2");
	arrow_ = CreateSprite(arrowData_, "white2x2");
	isPause_ = false;
	select_ = Retry;
}

void Pause::Update() {
	
	Input* input = Input::GetInstance();

	if (input->IsKeyTrigger(DIK_ESCAPE) || input->TriggerButton(XINPUT_GAMEPAD_START)) {
		isPause_ = !isPause_;
	}
	
	ToTitle_->SetIsActive(isPause_);
	retry_->SetIsActive(isPause_);
	grey_->SetIsActive(isPause_);
	arrow_->SetIsActive(isPause_);
	
	if (isPause_) {
		PauseUpdate();
	}
	else {
		UnPauseUpdate();
	}

#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Pause")) {
		if (ImGui::TreeNode("PauseData")) {

			DrawImGui(ToTitleData_, "ToTitleData_", ToTitle_.get());
			DrawImGui(retryData_, "retryData_", retry_.get());
			DrawImGui(greyData_, "greyData_", grey_.get());
			DrawImGui(arrowData_, "arrowData_", arrow_.get());

			if (ImGui::Button("Save")) {
				SaveJson();
			}
			ImGui::TreePop();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG

}

void Pause::PauseUpdate()
{

	int select = static_cast<int>(select_);


	if (Input::GetInstance()->IsKeyTrigger(DIK_DOWN) || Input::GetInstance()->DownRStick() || Input::GetInstance()->DownLStick()) {
		select++;
		select = std::clamp(select, 0, static_cast<int>(SelectNum) - 1);
	}

	if (Input::GetInstance()->IsKeyTrigger(DIK_UP) || Input::GetInstance()->UpRStick() || Input::GetInstance()->UpLStick()) {
		select--;
		select = std::clamp(select, 0, static_cast<int>(SelectNum) - 1);
	}

	select_ = static_cast<Select>(select);

	switch (select_)
	{
	case Pause::Retry:
		arrow_->SetPosition({ arrowData_.position.x , retry_->GetPosition().y });
		break;
	case Pause::ToTitle:
		arrow_->SetPosition({ arrowData_.position.x , ToTitle_->GetPosition().y });
		break;
	default:
		break;
	}

	if (Input::GetInstance()->IsKeyTrigger(DIK_SPACE) || Input::GetInstance()->PushButton(XINPUT_GAMEPAD_A)) {

		isPause_ = false;

		switch (select_)
		{
		case Pause::Retry:
			orderReset_ = true;
			break;
		case Pause::ToTitle:
			orderToTitle_ = true;
			break;
		default:
			break;
		}
	}
}

void Pause::UnPauseUpdate()
{
	orderReset_ = false;
	orderToTitle_ = false;
	select_ = Retry;
}

void Pause::LoadJson() {
	JSON_OPEN("Resources/Data/Pause/Pause.json");
	JSON_OBJECT("ToTitleData_");
	ToTitleData_.Load();
	JSON_OBJECT("retryData_");
	retryData_.Load();
	JSON_OBJECT("greyData_");
	greyData_.Load();
	JSON_OBJECT("arrowData_");
	arrowData_.Load();
	JSON_CLOSE();
}

void Pause::SaveJson() {
	JSON_OPEN("Resources/Data/Pause/Pause.json");
	JSON_OBJECT("ToTitleData_");
	ToTitleData_.Save();
	JSON_OBJECT("retryData_");
	retryData_.Save();
	JSON_OBJECT("greyData_");
	greyData_.Save();
	JSON_OBJECT("arrowData_");
	arrowData_.Save();
	JSON_CLOSE();
}

std::unique_ptr<Sprite> Pause::CreateSprite(const SpriteData& spriteData, std::string spriteName) {
	auto sprite = std::make_unique<Sprite>();
	sprite->SetTexture(ResourceManager::GetInstance()->FindTexture(spriteName));
	sprite->SetIsActive(true);
	sprite->SetDrawOrder(static_cast<uint8_t>(spriteData.order));
	sprite->SetScale(spriteData.scale);
	sprite->SetRotate(spriteData.rotate);
	sprite->SetPosition(spriteData.position);
	sprite->SetColor(spriteData.color);
	sprite->SetTexcoordBase(spriteData.textureBase);
	sprite->SetTexcoordSize(spriteData.textureSize);
	sprite->SetIsActive(false);
	return sprite;
}

void Pause::DrawImGui(SpriteData& spriteData, std::string string, Sprite* sprite) {
	sprite;
	spriteData;
#ifdef _DEBUG


	if (ImGui::TreeNode(string.c_str())) {
		ImGui::DragFloat2((string.c_str() + std::string(":scale")).c_str(), &spriteData.scale.x);
		ImGui::DragFloat((string.c_str() + std::string(":rotate")).c_str(), &spriteData.rotate, 0.1f);
		ImGui::DragFloat2((string.c_str() + std::string(":position")).c_str(), &spriteData.position.x);
		ImGui::DragFloat4((string.c_str() + std::string(":color")).c_str(), &spriteData.color.x, 0.1f, 0.0f, 1.0f);
		ImGui::DragFloat2((string.c_str() + std::string(":textureSize")).c_str(), &spriteData.textureSize.x);
		ImGui::DragFloat2((string.c_str() + std::string(":textureBase")).c_str(), &spriteData.textureBase.x);
		ImGui::DragInt((string.c_str() + std::string(":order")).c_str(), &spriteData.order);

		sprite->SetDrawOrder(static_cast<uint8_t>(spriteData.order));
		sprite->SetScale(spriteData.scale);
		sprite->SetRotate(spriteData.rotate);
		sprite->SetPosition(spriteData.position);
		sprite->SetColor(spriteData.color);
		sprite->SetTexcoordBase(spriteData.textureBase);
		sprite->SetTexcoordSize(spriteData.textureSize);


		ImGui::TreePop();
	}
#endif // _DEBUG
}

void Pause::SpriteData::Load() {
	JSON_LOAD(scale);
	JSON_LOAD(position);
	JSON_LOAD(rotate);
	JSON_LOAD(color);
	JSON_LOAD(textureSize);
	JSON_LOAD(textureBase);
	JSON_LOAD(order);
}

void Pause::SpriteData::Save() {
	JSON_SAVE(scale);
	JSON_SAVE(position);
	JSON_SAVE(rotate);
	JSON_SAVE(color);
	JSON_SAVE(textureSize);
	JSON_SAVE(textureBase);
	JSON_SAVE(order);
}
