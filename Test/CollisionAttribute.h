#pragma once

#include <cstdint>

namespace CollisionAttribute {
    // プレイヤー
    const uint32_t Player =                 0b1;
    // ボス
    const uint32_t Boss =                   0b10;
    // トリガー
    const uint32_t BossAttackTrigger =      0b100;
    // ゲームオブジェクト
    const uint32_t GameObject = 0b1000;
    const uint32_t Ground = 0b10000;
    const uint32_t Block = Ground;
    const uint32_t FireBarCenter = Ground;
    const uint32_t FireBarBar =             GameObject;
    const uint32_t Floor = Ground;
    const uint32_t PendulumBall =           GameObject;
    const uint32_t StageObject = Ground;
    const uint32_t RevengeCoin =            GameObject;
    const uint32_t Trap =                   GameObject;
    const uint32_t BeltConveyor =           GameObject;
    const uint32_t DropGimmickSwitch =      GameObject;
    const uint32_t DropGimmickDropper =     GameObject;
    const uint32_t DropGimmickDropperBall = GameObject;
}