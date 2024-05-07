#pragma once
namespace Character {
	enum State {
		// ボスのことを追いかける
		kChase,
		// ボスから逃げる
		kRunAway,
		// シーンチェンジ中
		kScneChange,


		kCount,
	};

	extern Character::State currentCharacterState_;
	extern Character::State preCharacterState_;
	extern Character::State nextCharacterState_;
	extern float time_;
	extern float toChaseTime_;
	extern float toRunAwayTime_;

	float GetSceneChangeTime();

	float GetSceneChangeT();
	// この関数を使いセットすること
	void SetNextScene(const Character::State& scene);
	// シーンチェンジに入った瞬間true
	bool IsSceneChange();
	void LoadJson();
	void SaveJson();
	void Update();
}
