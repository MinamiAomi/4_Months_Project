#pragma once

#include <cstdint>
#include <array>
#include "Graphics/Model.h"
#include "Math/Transform.h"
#include "Math/Random.h"
#include "Audio/AudioSource.h"
#include "Collision/GameObject.h"
class Player;
class Boss;
class UFO :
	public GameObject {
public:
	
	const int backFrame = 60;
	const int waitFrame = 360;
	const int disAppearFrame_ = 10;
	const int kFullFrame = backFrame + waitFrame + disAppearFrame_;

	void Initialize(Player* player,const Boss* boss);
	void Update();
	void UpdateTransform();

	void SetPlayer(Player* player) { player_ = player; }
	
	void SetIsActive();

	void SetIsInactive();

	bool GetIsActive() { return model_->IsActive(); }
	bool GetIsFree() { return isFreePlayer_; }

	ModelInstance& GetModel() { return *model_.get(); }

	const Player* GetPlayer() { return player_; }

private:

	Player* player_;
	const Boss* boss_;
	std::unique_ptr<ModelInstance> model_;
	Vector3 savePoint_[3];
	int frame = 0;
	bool isSaveDisAppearPos_ = false;
	bool isFreePlayer_ = true;
	Vector3 saveDisAppearPos_;

	std::unique_ptr<AudioSource> se_;
};