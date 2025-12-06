#pragma once
#include <json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

class config {
private:
    inline static json data;
    inline static fs::path config_path;
    inline static bool loaded = false;
public:
    static void init();
    static std::string get_string(const std::string& key, const std::string& default_val = "");
    static void set_string(const std::string& key, const std::string& value);

    static bool get_bool(const std::string& key, bool default_val = false);
    static void set_bool(const std::string& key, bool value);

    static json get_json(const std::string& key, const json& default_val = json::array());
    static void set_json(const std::string& key, const json& value);

    static void save();
};