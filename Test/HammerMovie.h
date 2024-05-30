#pragma once

#include <array>
#include <string>
#include <memory>

#include "Graphics/Sprite.h"
#include "Movie.h"
#include "Audio/AudioSource.h"


class HammerMovie : public Movie{
public:

	const int kZoomFrame = 40;
	const int kStopFrame = 10;
	const int kBackFrame = 40;
	const int kFullFrame = kZoomFrame + kStopFrame + kBackFrame;

	void Initialize(Player* player, Boss* boss, Camera* camera, const StageCamera* stageCamera) override;
	void Update() override;
	void Reset() override;
private:
	Vector3 curvePoint_[3];
	Quaternion saveQuaternion_;
	std::unique_ptr<AudioSource> se_;
	bool isPlaySe_ = false;
};