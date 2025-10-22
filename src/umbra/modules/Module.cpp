#include "Module.hpp"
#include "stc/Environment.hpp"

namespace umbra {

Module::Module() : parseTransform(parseCtx) {

}

std::string Module::getEnvWithTransform(
    const std::string& name,
    const std::string& defaultVal
) {
    auto env = stc::getEnv(name.c_str(), defaultVal);
    return parse::parse(
        env,
        this->parseCtx
    );
}

}
