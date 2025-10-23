#pragma once

#include "umbra/util/FilesystemExt.hpp"
#include <string>

namespace umbra::parse {

/**
 * Contains common context that should only be initialised once per call.
 */
struct CommonContext {
    std::optional<std::string> gitRoot = util::getGitRoot();
};

/**
 * Contains per-module context, as well as a reference to the global CommonContext
 */
struct ParseContext {
    const CommonContext& commonContext;
};

extern std::string parse(const std::string& in, const ParseContext& context);

}
