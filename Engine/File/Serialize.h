#pragma once

#include <string>
#include <filesystem>

#include "Math/MathUtils.h"

namespace JsonHelper {

    bool Open(const std::filesystem::path& path);
    bool Close();

    void Serialize(const uint32_t& value, const std::string& name);
    void Serialize(const int32_t& value, const std::string& name);
    void Serialize(const float& value, const std::string& name);
    void Serialize(const Vector2& value, const std::string& name);
    void Serialize(const Vector3& value, const std::string& name);
    void Serialize(const Vector4& value, const std::string& name);
    void Serialize(const Quaternion& value, const std::string& name);
    void Serialize(const std::string& value, const std::string& name);

    void Deserialize(uint32_t& value, const std::string& name);
    void Deserialize(int32_t& value, const std::string& name);
    void Deserialize(float& value, const std::string& name);
    void Deserialize(Vector2& value, const std::string& name);
    void Deserialize(Vector3& value, const std::string& name);
    void Deserialize(Vector4& value, const std::string& name);
    void Deserialize(Quaternion& value, const std::string& name);
    void Deserialize(std::string& value, const std::string& name);
}


#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)


#define JSON_OPEN(x) (JsonHelper::Open(x))
#define JSON_CLOSE() (JsonHelper::Close())
#define JSON_SERIALIZE(x) (JsonHelper::Serialize(x,  STRINGIFY(x)))
#define JSON_DESERIALIZE(x) (JsonHelper::Deserialize(x,  STRINGIFY(x)))


