#pragma once
namespace Character {
    enum State {
        // ボスのことを追いかける
        kChase,
        // ボスから逃げる
        kRunAway,
    };
}

extern Character::State characterState_;