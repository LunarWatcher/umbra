#pragma once

#include "stc/Environment.hpp"
#include <string>

namespace test {

/**
 * Utility for wiping environment variables
 */
struct EnvVar {
    std::string name;

    ~EnvVar() {
        stc::setEnv(name.c_str(), nullptr);
    }
};

}
