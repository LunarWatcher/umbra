#pragma once

#include "umbra/util/FilesystemExt.hpp"
#include <optional>
#include <string>

namespace umbra::parse {

struct ParseContext {
    std::optional<std::string> gitRoot = umbra::util::getGitRoot();
};

extern std::string parse(const std::string& in, const ParseContext& context);

}
