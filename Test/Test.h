#pragma once
#include "Framework/Game.h"

class Test :
    public Game {
public:

    void OnInitialize() override;
    void OnFinalize() override;
    bool IsTerminated() override { return resourcesLoaded_; }

    bool ResourcesLoaded() const { return resourcesLoaded_; }

private:
    void LoadResources();

    bool resourcesLoaded_;
};