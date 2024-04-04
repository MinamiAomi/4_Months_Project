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
        std::ifstream file;
        file.open(path);
        // 開けなかった
        assert(!file.fail());

        file >> root;
        file.close();

        openPath = path;

        target = &root;

        return root.empty();
    }

    bool Object(const std::string& name) {
        assert(target);

        // rootに挿入する
        if (name.empty()) {
            target = &root;
        }
        // オブジェクトを追加
        else {
            target->emplace(name);
            target = &target->at(name);
        }

        return false;
    }

    bool Close() {
        assert(!openPath.empty());
        assert(target);

        std::ofstream file;
        file.open(openPath);
        // 開けなかった
        assert(!file.fail());

        file << std::setw(4) << root << std::endl;

        file.close();
        openPath.clear();

        root.clear();
        target = nullptr;

        return false;
    }


}