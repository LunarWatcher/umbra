#include "ConfigSpec.hpp"
#include <stdexcept>

namespace umbra {

void devenv::from_json(const nlohmann::json& src, ConfigSpec& dest) {
    if (!src.is_object()) {
        throw std::runtime_error("Expected root node to be object");
    }
    if (src.contains("envs")) {
        for (auto& [env, spec] : src.at("envs").items()) {
            EnvConfig conf = spec;
            dest.envs[env] = conf;

            if (conf.aliasFor.has_value() && env != "default") {
                throw std::runtime_error("alias-for is only allowed for the default environment");
            }
        }
    } 
    if (src.contains("env")) {
        throw std::runtime_error("Your config file contains the \"env\" key. This is a typo, the correct key is envs");
    }

    // Default must always exist, but it doesn't have to exist here
    if (!dest.envs.contains("default")) {
        dest.envs["default"] = {};
    }
}

void devenv::from_json(const nlohmann::json& src, EnvConfig& dest) {
    if (src.contains("alias-for")) {
        dest.aliasFor = src.at("alias-for").get<std::string>();
    }
    if (src.contains("vars")) {
        src.at("vars").get_to(dest.vars);
    }

    if (src.contains("prepend")) {
        src.at("prepend").get_to(dest.prepend);
    }
}

}
