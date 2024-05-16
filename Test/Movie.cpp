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