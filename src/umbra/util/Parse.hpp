#pragma once

#include <string>

namespace umbra::parse {

/**
 * Currently intentionally left empty. Will likely be populated in the future.
 * Largely exists now because signature refactoring annoying (really need to look into comby at some point)
 */
struct ParseContext {};

extern std::string parse(const std::string& in, const ParseContext& context);

}
