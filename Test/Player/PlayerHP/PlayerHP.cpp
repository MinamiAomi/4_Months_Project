#include "PlayerHP.h"

void PlayerHP::Initialize() {
	Reset();
}

void PlayerHP::Update() {

}

void PlayerHP::Reset() {
	currentHP_ = kMaxHP;
}
