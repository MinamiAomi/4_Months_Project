#include "BossHP.h"

void BossHP::Initialize() {
	Reset();
}

void BossHP::Update() {}

void BossHP::Reset() {
	currentHP_ = kMaxHP;
}
