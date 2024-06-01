#pragma once

#include <memory>

#include "SceneTransition.h"

class BaseScene;

class SceneManager {
public:
    static SceneManager* GetInstance();

    void Update();
    void Finalize();

    template<class T>
    T* ChangeScene(bool useTransition = true) {
        static_assert(std::is_base_of<BaseScene, T>::value, "BaseSceneを継承していません。");
        T* tmpScene = nullptr;
        // 次のシーンがあるまたはトランジションしていない
        if (!nextScene_ && sceneTransition_.GetMode() == SceneTransition::Mode::Ready) {
            std::unique_ptr<T> nextScene = std::make_unique<T>();
            tmpScene = nextScene.get();
            nextScene_ = std::move(nextScene);
            if (useTransition) {
                sceneTransition_.Start(SceneTransition::Mode::In);
            }
        }
        return tmpScene;
    }

    SceneTransition& GetSceneTransition() { return sceneTransition_; }

private:
    SceneManager();
    ~SceneManager();
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    std::unique_ptr<BaseScene> currentScene_;
    std::unique_ptr<BaseScene> nextScene_;
    SceneTransition sceneTransition_;

};