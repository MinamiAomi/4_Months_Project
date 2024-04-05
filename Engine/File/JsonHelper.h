#pragma once

#include <string>
#include <filesystem>

#include "Math/MathUtils.h"

namespace JsonHelper {

    bool Open(const std::filesystem::path& path);
    bool Object(const std::string& name = "");
    void Close();

    void Save(const bool& value, const std::string& name);
    void Save(const int32_t& value, const std::string& name);
    void Save(const uint32_t& value, const std::string& name);
    void Save(const float& value, const std::string& name);
    void Save(const Vector2& value, const std::string& name);
    void Save(const Vector3& value, const std::string& name);
    void Save(const Vector4& value, const std::string& name);
    void Save(const Quaternion& value, const std::string& name);
    void Save(const std::string& value, const std::string& name);

    bool Load(bool& value, const std::string& name);
    bool Load(int32_t& value, const std::string& name);
    bool Load(uint32_t& value, const std::string& name);
    bool Load(float& value, const std::string& name);
    bool Load(Vector2& value, const std::string& name);
    bool Load(Vector3& value, const std::string& name);
    bool Load(Vector4& value, const std::string& name);
    bool Load(Quaternion& value, const std::string& name);
    bool Load(std::string& value, const std::string& name);
}

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

/// ファイルを開く
/// 必ず最初に呼び出す
/// x : ファイルのパス 
/// ファイルがもともとある時はTrue
#define JSON_OPEN(x) (JsonHelper::Open(x))
/// ファイルを閉じる
/// 必ず最後に呼び出す
#define JSON_CLOSE() (JsonHelper::Close())
// Jsonのルートに戻る
#define JSON_ROOT() (JsonHelper::Object())
// Objectに入る
// x : Objectの名前
// Objectがある時True
#define JSON_OBJECT(x) (JsonHelper::Object(x))
// 引数に与えた変数の名前でセーブ
#define JSON_SAVE(x) (JsonHelper::Save(x,  STRINGIFY(x)))
// 引数に与えた変数の名前でロード
// 読み込めたらTrue
#define JSON_LOAD(x) (JsonHelper::Load(x,  STRINGIFY(x)))


