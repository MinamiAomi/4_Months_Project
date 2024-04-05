#include "JsonHelper.h"

#include <fstream>
#include <cassert>

#include "Externals/nlohmann/json.hpp"

namespace JsonHelper {

    static std::filesystem::path openPath;
    static nlohmann::json root;
    static nlohmann::json* target;

    bool Open(const std::filesystem::path& path) {
        assert(!target);
        // クリア
        root.clear();
        if (std::filesystem::exists(path)) {
            // ファイルを開く
            std::ifstream file(path);
            // 開けなかった
            assert(!file.fail());
            // 読み込む
            file >> root;
        }
        openPath = path;
        target = &root;
        // ファイルがあったか
        return !root.empty();
    }

    bool Object(const std::string& name) {
        assert(target);

        bool contains = false;

        // rootに挿入する
        if (name.empty()) {
            target = &root;
        }
        // オブジェクトを追加
        else {
            contains = target->contains(name);
            // まだObjectがない
            if (!contains) {
                // 新しいオブジェクトを追加
                target->emplace(name, nlohmann::json::object());
            }
            // 追加したオブジェクトが次のターゲット
            target = &target->at(name);
        }
        return contains;
    }

    void Close() {
        assert(!openPath.empty());
        assert(target);
        if (!root.empty()) {
            // ファイルを開く
            std::ofstream file(openPath);
            // 開けなかった
            assert(!file.fail());
            // 書き込む
            file << std::setw(4) << root << std::endl;
        }
        // 消す
        openPath.clear();
        root.clear();
        target = nullptr;
    }

    void Save(const bool& value, const std::string& name) {
        (*target)[name] = value;
    }

    void Save(const int32_t& value, const std::string& name) {
        (*target)[name] = value;
    }

    void Save(const uint32_t& value, const std::string& name) {
        (*target)[name] = value;
    }

    void Save(const float& value, const std::string& name) {
        (*target)[name] = value;
    }

    void Save(const Vector2& value, const std::string& name) {
        (*target)[name] = nlohmann::json::array({ value.x, value.y });
    }

    void Save(const Vector3& value, const std::string& name) {
        (*target)[name] = nlohmann::json::array({ value.x, value.y, value.z });
    }

    void Save(const Vector4& value, const std::string& name) {
        (*target)[name] = nlohmann::json::array({ value.x, value.y, value.z, value.w });
    }

    void Save(const Quaternion& value, const std::string& name) {
        (*target)[name] = nlohmann::json::array({ value.x, value.y, value.z, value.w });
    }

    void Save(const std::string& value, const std::string& name) {
        (*target)[name] = value;
    }

    void Load(bool& value, const std::string& name) {
        auto iter = target->find(name);
        // ファイルにデータがない
        assert(iter != target->end());
        // bool型
        assert(iter->is_boolean());
        value = iter->get<bool>();
    }

    void Load(int32_t& value, const std::string& name) {
        auto iter = target->find(name);
        // ファイルにデータがない
        assert(iter != target->end());
        // 整数型
        assert(iter->is_number_integer());
        value = iter->get<int32_t>();
    }

    void Load(uint32_t& value, const std::string& name) {
        auto iter = target->find(name);
        // ファイルにデータがない
        assert(iter != target->end());
        // 符号なし整数型
        assert(iter->is_number_unsigned());
        value = iter->get<uint32_t>();
    }

    void Load(float& value, const std::string& name) {
        auto iter = target->find(name);
        // ファイルにデータがない
        assert(iter != target->end());
        // 符号なし整数型
        assert(iter->is_number_float());
        value = iter->get<float>();
    }

    void Load(Vector2& value, const std::string& name) {
        auto iter = target->find(name);
        // ファイルにデータがない
        assert(iter != target->end());
        // 配列型かつ要素2
        assert(iter->is_array() && iter->size() == 2);
        value.x = iter->at(0);
        value.y = iter->at(1);
    }

    void Load(Vector3& value, const std::string& name) {
        auto iter = target->find(name);
        // ファイルにデータがない
        assert(iter != target->end());
        // 配列型かつ要素3
        assert(iter->is_array() && iter->size() == 3);
        value.x = iter->at(0);
        value.y = iter->at(1);
        value.z = iter->at(2);
    }

    void Load(Vector4& value, const std::string& name) {
        auto iter = target->find(name);
        // ファイルにデータがない
        assert(iter != target->end());
        // 配列型かつ要素4
        assert(iter->is_array() && iter->size() == 4);
        value.x = iter->at(0);
        value.y = iter->at(1);
        value.z = iter->at(2);
        value.w = iter->at(3);
    }

    void Load(Quaternion& value, const std::string& name) {
        auto iter = target->find(name);
        // ファイルにデータがない
        assert(iter != target->end());
        // 配列型かつ要素4
        assert(iter->is_array() && iter->size() == 4);
        value.x = iter->at(0);
        value.y = iter->at(1);
        value.z = iter->at(2);
        value.w = iter->at(3);
    }

    void Load(std::string& value, const std::string& name) {
        auto iter = target->find(name);
        // ファイルにデータがない
        assert(iter != target->end());
        // 配列型かつ要素4
        assert(iter->is_string());
        value = iter->get<std::string>();
    }

}