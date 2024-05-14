#pragma once

namespace GameSpeed {
	extern float chaseSpeed;
	extern float runAwaySpeed;
	extern float currentSpeed;

	float GetGameSpeed();
	void LoadJson();
	void SaveJson();
	void Update();
}
