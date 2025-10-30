#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace umbra::devenv {

struct EnvConfig {
    std::optional<std::string> aliasFor;
    std::map<std::string, std::string> vars;
    std::map<std::string, std::vector<std::string>> prepend;
};

struct ConfigSpec {
    std::map<std::string, EnvConfig> envs;
};

extern void from_json(const nlohmann::json& src, ConfigSpec& dest);
extern void from_json(const nlohmann::json& src, EnvConfig& dest);

}
