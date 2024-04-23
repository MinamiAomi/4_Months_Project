#pragma once

namespace GameSpeed {
	extern float chaseSpeed;
	extern float runAwaySpeed;

	float GetGameSpeed();
	void LoadJson();
	void SaveJson();
	void Update();
}
