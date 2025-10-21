#pragma once

#include <optional>
#include <string>
#include <stc/Environment.hpp>

namespace umbra::util {

extern std::optional<std::string> getGitRoot();
/**
 * Does a lookup on the provided paths. 
 *
 * \param paths Roots used for the search
 * \param filenames The filenames to search for.
 */
extern std::vector<std::filesystem::path> findMatchesInPaths(
    const std::vector<std::filesystem::path>& paths,
    const std::vector<std::string>& filenames,
    bool stopAfterFirst = false
);

}
