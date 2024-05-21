#include "Movie.h"


#include "File/JsonHelper.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

bool Movie::isPlaying = false;

void Movie::Initialize(Player* player, Boss* boss, Camera* camera, const StageCamera* stageCamera) {
	player_ = player;
	boss_ = boss;
	camera_ = camera;
	stageCamera_ = stageCamera;

	isPlaying = false;
	isSetCamera_ = false;
	frame_ = 0;
}

void Movie::Update()
{
	if (!isSetCamera_) {
		isSetCamera_ = true;
		camera_->SetPosition(stageCamera_->GetCamera()->GetPosition());
		camera_->SetRotate(stageCamera_->GetCamera()->GetRotate());
	}
}

void Movie::Reset()
{
	isPlaying = false;
	isSetCamera_ = false;
	isInitialize_ = false;
	frame_ = 0;
}

void Movie::ZoomInOut(const Vector3& savePos, const Vector3& axis, const float t)
{
	float newT = t / 0.3f;
	newT = std::clamp(newT, 0.0f, 1.0f);
	if (newT < 1.0f) {
		Vector3 result = savePos + Vector3::Lerp(newT, { 0.0f,0.0f,0.0f }, axis * 7.0f);
		camera_->SetPosition(result);
	}
	else {
		newT = (t - 0.3f) / 0.7f;
		newT = std::clamp(newT, 0.0f, 1.0f);
		Vector3 result = savePos + Vector3::Lerp(newT, axis * 7.0f,{ 0.0f,0.0f,0.0f });
		camera_->SetPosition(result);
	}
}
