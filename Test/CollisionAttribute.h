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
    const uint32_t DropGimmickDropperBall = 0b100000;
}