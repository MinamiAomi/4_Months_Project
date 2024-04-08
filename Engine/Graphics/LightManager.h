#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Math/MathUtils.h"


class DirectionalLight {
public:
    DirectionalLight() : color(Vector3::one), direction(Vector3::down), intensity(1.0f) {}
    void DrawImGui(const std::string& label);

    Vector3 color;
    float intensity;
    Vector3 direction;
};

class PointLight {
public:
    PointLight() : color(Vector3::one), position(Vector3::zero), intensity(0.0f), range(1.0f), decay(1.0f) {};

    Vector3 color;
    float intensity;
    Vector3 position;
    float range;
    float decay;
};

class SpotLight {
public:

};

class LineLight {
public:
    LineLight() : color(Vector3::one), origin(Vector3::zero), diff(Vector3::unitZ), intensity(0.0f), range(1.0f), decay(1.0f) {};

    Vector3 color;
    float intensity;
    Vector3 origin;
    float range;
    Vector3 diff;
    float decay;
};

class LightManager {
public:
    void Add(const std::shared_ptr<DirectionalLight>& directionalLight) { directionalLights_.emplace_back(directionalLight); }
    void Add(const std::shared_ptr<PointLight>& pointLight) { pointLights_.emplace_back(pointLight); }
    void Add(const std::shared_ptr<LineLight>& lineLight) { lineLights_.emplace_back(lineLight); }

    std::vector<std::shared_ptr<DirectionalLight>> GetDirectionalLight() const { return directionalLights_; }
    std::vector<std::shared_ptr<PointLight>> GetPointLight() const { return pointLights_; }
    std::vector<std::shared_ptr<LineLight>> GetLineLight() const { return lineLights_; }

    void Reset() {
        directionalLights_.clear();
        pointLights_.clear();
        lineLights_.clear();
    }

private:
    std::vector<std::shared_ptr<DirectionalLight>> directionalLights_;
    std::vector<std::shared_ptr<PointLight>> pointLights_;
    std::vector<std::shared_ptr<LineLight>> lineLights_;
};